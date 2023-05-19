#pragma once

#include <string>

#include <core/input/InputState.h>
#include <core/input/Key.h>
#include <core/input/Mouse.h>
#include <core/math/Vector2.h>

namespace Core {
// Given to Client code to allow for simple querying of input state
class Input {
    InputState& m_input_state;

  public:
    Input(InputState& input_state) : m_input_state(input_state) {}

    bool IsPressed(Key key) const;
    bool IsPressed(Mouse button) const;

    bool IsReleased(Key key) const;
    bool IsReleased(Mouse button) const;

    bool IsHeld(Key key) const;
    bool IsHeld(Mouse button) const;

    bool IsDown(Key key) const;
    bool IsDown(Mouse button) const;

    float GetMouseX() const;
    float GetMouseY() const;
    Vector2 GetMousePosition() const;

    float GetMouseDeltaX() const;
    float GetMouseDeltaY() const;

    float GetScrollDeltaX() const;
    float GetScrollDeltaY() const;

    std::string GetText() const;
};
}