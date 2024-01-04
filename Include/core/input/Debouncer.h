#pragma once

#include <core/Controller.h>
#include <core/chrono/Duration.h>
#include <core/chrono/Timer.h>
#include <core/input/Key.h>
#include <functional>

namespace Core {

struct Debouncer {
    enum class DebouncerState {
        Idle,
        Pressed,
        Held
    };

    DebouncerState state = DebouncerState::Idle;
    Duration current_timer = Duration(0);
    const Core::Duration wait_time = Duration::FromMilliseconds(250);
    Timer wait_timer = Timer(wait_time);

  protected:
    virtual bool InputIsPressed(Input& input) = 0;
    virtual bool InputIsHeld(Input& input) = 0;
    virtual bool InputIsReleased(Input& input) = 0;

  public:
    virtual ~Debouncer() = default;

    bool Update(Controller& controller)
    {
        auto& chrono = controller.GetChronometer();
        auto& input = controller.GetInput();

        switch (state) {
        case DebouncerState::Idle:
            if (InputIsPressed(input)) {
                state = DebouncerState::Pressed;
                current_timer = Duration(0);
            }
            break;
        case DebouncerState::Pressed:
            if (InputIsHeld(input)) {
                current_timer = current_timer + chrono.GetDelta();
                if (current_timer > Duration::FromMilliseconds(250)) {
                    state = DebouncerState::Held;
                }
            } else {
                state = DebouncerState::Idle;
            }
            break;
        case DebouncerState::Held:
            if (InputIsReleased(input)) {
                state = DebouncerState::Idle;
            }
            break;
        default: break;
        }

        bool is_up_time = wait_timer.Update(chrono);
        bool is_held = state == DebouncerState::Held;
        if (is_held && is_up_time) {
            return true;
        }

        return InputIsPressed(input);
    }
};

class KeyDebouncer : public Debouncer {
  protected:
    bool InputIsPressed(Input& input) { return input.IsPressed(matching_key); }
    bool InputIsHeld(Input& input) { return input.IsHeld(matching_key); }
    bool InputIsReleased(Input& input) { return input.IsReleased(matching_key); }

    Key matching_key;

  public:
    KeyDebouncer(Key matching_key) : matching_key(matching_key) {}

    Key GetKey() { return matching_key; }
};

class MouseDebouncer : public Debouncer{
  protected:
    bool InputIsPressed(Input& input) { return input.IsPressed(mouse_button); }
    bool InputIsHeld(Input& input) { return input.IsHeld(mouse_button); }
    bool InputIsReleased(Input& input) { return input.IsReleased(mouse_button); }

    Mouse mouse_button = Mouse::LEFT;

  public:
    MouseDebouncer(Mouse mouse_button) : mouse_button(mouse_button) {}

    Mouse GetMouse() { return mouse_button; }
};
} // namespace Core::Gui