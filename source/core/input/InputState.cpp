#include <core/input/InputState.h>

namespace Core {

InputState::InputState()
{
    for (unsigned i = 0; i < 256; i++) {
        current_keys[i] = false;
        previous_keys[i] = false;
    }

    for (unsigned i = 0; i < 3; i++) {
        current_mouse[i] = false;
        previous_mouse[i] = false;
    }

    current_mouse_x = 0;
    current_mouse_y = 0;
    last_mouse_x = 0;
    last_mouse_y = 0;

    scroll_delta = 0;
}

void InputState::Update()
{
    for (unsigned i = 0; i < 256; i++) {
        previous_keys[i] = current_keys[i];
    }

    for (unsigned i = 0; i < 3; i++) {
        previous_mouse[i] = current_mouse[i];
    }

    last_mouse_x = current_mouse_x;
    last_mouse_y = current_mouse_y;
}

void InputState::Clear()
{
    char_buffer.clear();
    scroll_delta = 0;
}
} // namespace Core