#include <Window.h>

Vector2 Component::GetPosition() const
{
    return m_position;
}

Vector2 Component::GetSize() const
{
    return m_size;
}

void Component::SetPosition(unsigned x, unsigned y)
{
    m_position = Vector2(x, y);
}

void Component::SetSize(unsigned width, unsigned height)
{
    m_size = Vector2(width, height);
}

void Window::Update(FocusManager &manager, State &state, FontGraphics &font_graphics, Pallette pallette)
{
    if (toggle_shortcut(state.input))
    {
        is_open = !is_open;
    }

    if (is_open)
    {
        // Update the title bar and update this window's position in response
        title_bar.Update(manager, state, *this, pallette, font_graphics, title);
        SetPosition(title_bar.GetPosition().x, title_bar.GetPosition().y + title_bar.GetSize().y);

        // If the mouse is clicking in the body of the window, request focus
        Vector2 mouse_position = Vector2(state.input.GetMouseX(), state.input.GetMouseY());
        bool is_inside = Vector2::IsInBounds(mouse_position, GetPosition(), GetSize());
        if (is_inside && state.input.IsPressed(MouseButton::LEFT))
        {
            manager.RequestFocus(this);
        }

        OnUpdate(manager, state, font_graphics, pallette);

        // Update the root panel to match the window's position and size
        root_panel->SetPosition(GetPosition().x, GetPosition().y);
        root_panel->SetSize(GetSize().x, GetSize().y);
        root_panel->Update(state);
    }
}

void DragBar::Update(FocusManager &manager, State &state, Window &parent, Pallette pallette,
                     FontGraphics &font_graphics, std::string title)
{
    // @brief: updates the window title and vieware position based on the dragging input
    // @returns: the new position of the parent window which will change if the title bar is being dragged
    Vector2 parent_position = parent.GetPosition();
    Vector2 parent_size = parent.GetSize();

    Input &input = state.input;
    Graphics &graphics = state.graphics;

    // Sync the position of the title bar with the parent window
    SetPosition(parent_position.x, parent_position.y - GetSize().y);

    // If the mouse is dragging in the title bar, move the console window
    bool is_held = input.IsHeld(MouseButton::LEFT);
    Vector2 mouse_position = Vector2(input.GetMouseX(), input.GetMouseY());
    bool is_inside = Vector2::IsInBounds(mouse_position, GetPosition(), GetSize());

    // This works much better because it causes the title bar to act stickily
    // Once we have detecting any action inside, stick the window to the mouse
    // Once mouse is released, unstick the window irrespective of mouse position
    if (!is_held)
    {
        is_being_dragged = false;
    }

    if (is_held && is_inside)
    {
        is_being_dragged = true;
    }

    // If the mouse is clicking in the title bar, request focus
    if (input.IsPressed(MouseButton::LEFT) && is_inside)
    {
        manager.RequestFocus(&parent);
    }

    int x = parent_position.x;
    int y = parent_position.y;

    if (is_being_dragged && manager.HasFocus(&parent))
    {
        x = input.GetMouseX() - (parent_size.x / 2);
        y = input.GetMouseY() + (GetSize().y / 2);
        x = std::clamp(x, 0, (int)(graphics.GetWidth() - parent_size.x));
        y = std::clamp(y, (int)GetSize().y, (int)(graphics.GetHeight() - (parent_size.y)));
        SetPosition(x, y - GetSize().y);
    }

    // Draw the title bar
    Pixel color = manager.HasFocus(&parent) ? pallette.highlight : pallette.highlight.Darker();
    graphics.FillRect(color, x, y - GetSize().y, parent_size.x, GetSize().y);

    // Draw the title text centered in the title bar
    Vector2 text_size = font_graphics.MeasureString(title);
    int text_x = x + (parent_size.x / 2) - (text_size.x / 2);
    int text_y = y - GetSize().y + (GetSize().y / 2) - (text_size.y / 2);
    font_graphics.DrawString(pallette.foreground, title, text_x, text_y);
    graphics.DrawRect(pallette.border, x, y - GetSize().y, parent_size.x, GetSize().y);
};

void ConsoleWindow::OnUpdate(FocusManager &manager, State &state, FontGraphics &font_graphics, Pallette pallette)
{

    if (manager.HasFocus(this))
    {
        text_type.Update(state.input, state.chrono);

        scroll_up_debouncer.Update(state.input, state.chrono);
        if (state.input.IsPressed(Key::Up) || scroll_up_debouncer.IsHeld())
        {
            scroll_pixel_offset -= 1;
        }

        scroll_down_debouncer.Update(state.input, state.chrono);
        if (state.input.IsPressed(Key::Down) || scroll_down_debouncer.IsHeld())
        {
            scroll_pixel_offset += 1;
        }

        if (scroll_pixel_offset < 0)
        {
            scroll_pixel_offset = 0;
        }
    }

    unsigned x = GetPosition().x;
    unsigned y = GetPosition().y;
    unsigned width = GetSize().x;
    unsigned height = GetSize().y;

    // Draw the background
    state.graphics.FillRect(pallette.background, x, y, width, height);
    state.graphics.DrawRect(pallette.border, x, y, width, height);

    TextRenderer text_render(x + padding, y + padding, width - padding, height - padding, padding,
                             scroll_pixel_offset);
    text_render.DrawText(pallette.foreground, text_type.GetText(), font_graphics);

    // Update the cursor and draw it to the screen
    if (manager.HasFocus(this) && cursor_timer.Update(state.chrono))
    {
        cursor_character = cursor_character == ' ' ? '_' : ' ';
    }

    std::string cursor_string = std::string(1, this->cursor_character);
    text_render.DrawText(pallette.foreground, cursor_string, font_graphics);
}

void FocusManager::Update(State &state, FontGraphics &font_graphics, Pallette pallette)
{
    has_given_focus = false;

    for (Window *window : windows)
    {
        if (window != active_window)
            window->Update(*this, state, font_graphics, pallette);
    }

    if (active_window != nullptr)
        active_window->Update(*this, state, font_graphics, pallette);
}

void FocusManager::RegisterWindow(Window *window)
{
    windows.push_back(window);
}

void FocusManager::RequestFocus(Window *window)
{
    if (!has_given_focus)
    {
        active_window = window;
        has_given_focus = true;
    }
}

bool FocusManager::HasFocus(Window *window)
{
    return active_window == window;
}