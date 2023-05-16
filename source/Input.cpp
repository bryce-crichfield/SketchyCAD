#include <Engine.h>

InputState::InputState()
{
    for (unsigned i = 0; i < 256; i++)
    {
        current_keys[i] = false;
        previous_keys[i] = false;
    }

    for (unsigned i = 0; i < 3; i++)
    {
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
    for (unsigned i = 0; i < 256; i++)
    {
        previous_keys[i] = current_keys[i];
    }

    for (unsigned i = 0; i < 3; i++)
    {
        previous_mouse[i] = current_mouse[i];
    }

    last_mouse_x = current_mouse_x;
    last_mouse_y = current_mouse_y;

}

void InputState::Clear() {
    char_buffer.clear();
    scroll_delta = 0;
}

bool Input::IsPressed(Key key) const
{
    return m_input_state.current_keys[(unsigned)key] && !m_input_state.previous_keys[(unsigned)key];
}

bool Input::IsPressed(MouseButton button) const
{
    return m_input_state.current_mouse[(unsigned)button] && !m_input_state.previous_mouse[(unsigned)button];
}

bool Input::IsReleased(Key key) const
{
    return !m_input_state.current_keys[(unsigned)key] && m_input_state.previous_keys[(unsigned)key];
}

bool Input::IsReleased(MouseButton button) const
{
    return !m_input_state.current_mouse[(unsigned)button] && m_input_state.previous_mouse[(unsigned)button];
}

bool Input::IsHeld(Key key) const
{
    return m_input_state.current_keys[(unsigned)key];
}

bool Input::IsHeld(MouseButton button) const
{
    return m_input_state.current_mouse[(unsigned)button];
}

bool Input::IsDown(Key key) const
{
    return m_input_state.current_keys[(unsigned)key];
}

bool Input::IsDown(MouseButton button) const
{
    return m_input_state.current_mouse[(unsigned)button];
}

float Input::GetMouseX() const
{
    return m_input_state.current_mouse_x;
}

float Input::GetMouseY() const
{
    return m_input_state.current_mouse_y;
}

Vector2 Input::GetMousePosition() const
{
    return Vector2(m_input_state.current_mouse_x, m_input_state.current_mouse_y);
}

float Input::GetMouseDeltaX() const
{
    return m_input_state.current_mouse_x - m_input_state.last_mouse_x;
}

float Input::GetMouseDeltaY() const
{
    return m_input_state.current_mouse_y - m_input_state.last_mouse_y;
}

float Input::GetScrollDeltaX() const
{
    return 0;
}

float Input::GetScrollDeltaY() const
{
    return m_input_state.scroll_delta;
}

std::string Input::GetText() const
{
    return std::string(m_input_state.char_buffer.begin(), m_input_state.char_buffer.end());
}