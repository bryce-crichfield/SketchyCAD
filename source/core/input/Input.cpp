#include <core/input/Input.h>
#include <core/input/InputState.h>
#include <algorithm>

#include <iostream>

namespace Core {

bool Input::IsPressed(Key key) const
{
    return m_input_state.current_keys[(unsigned)key] && !m_input_state.previous_keys[(unsigned)key];
}

bool Input::IsPressed(Mouse button) const
{
    return m_input_state.current_mouse[(unsigned)button] && !m_input_state.previous_mouse[(unsigned)button];
}

bool Input::IsReleased(Key key) const
{
    return !m_input_state.current_keys[(unsigned)key] && m_input_state.previous_keys[(unsigned)key];
}

bool Input::IsReleased(Mouse button) const
{
    return !m_input_state.current_mouse[(unsigned)button] && m_input_state.previous_mouse[(unsigned)button];
}

bool Input::IsHeld(Key key) const
{
    return m_input_state.current_keys[(unsigned)key];
}

bool Input::IsHeld(Mouse button) const
{
    return m_input_state.current_mouse[(unsigned)button];
}

bool Input::IsDown(Key key) const
{
    return m_input_state.current_keys[(unsigned)key];
}

bool Input::IsDown(Mouse button) const
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
}