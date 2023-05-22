#include <core/gui/Window.h>
#include <core/font/FontGraphics.h>
#include <iostream>
namespace Core::Gui {
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

void DrawCursor(Controller& controller) {
    Graphics& graphics = controller.GetGraphics();
    Input& input = controller.GetInput();

    unsigned x = input.GetMouseX();
    unsigned y = input.GetMouseY();

    float x0 = x;
    float y0 = y;
    float x1 = x;
    float y1 = y + 5;
    float x2 = x + 4;
    float y2 = y + 4;

    graphics.DrawLine(Color::WHITE, x0, y0, x1, y1);
    graphics.DrawLine(Color::WHITE, x1, y1, x2, y2);
    graphics.DrawLine(Color::WHITE, x2, y2, x0, y0);
}

void Window::Update(Controller& controller)
{
    // WindowManager &manager = state.static_context.window_manager;
    
    if (m_toggle_shortcut(controller.GetInput()))
    {
        if (!static_interactor.HasLock()) {
            static_interactor.ForceLock();
        } else {
            std::cout << "Releasing lock" << std::endl;
            static_interactor.ReleaseLock();
        }

    }

    // If the mouse is clicking in the body of the window, request focus
    auto& input = controller.GetInput();
    Vector2 mouse_position = Vector2(input.GetMouseX(), input.GetMouseY());
    bool is_inside = Vector2::IsInBounds(mouse_position, GetPosition(), GetSize());

    if (static_interactor.HasLock())
    {
        // Update the title bar and update this window's position in response
        m_drag_bar.Update(controller, *this, m_title);
        SetPosition(m_drag_bar.GetPosition().x, m_drag_bar.GetPosition().y + m_drag_bar.GetSize().y);
    }

    // Update the root panel to match the window's position and size
    m_root_panel->SetPosition(GetPosition().x, GetPosition().y);
    m_root_panel->SetSize(GetSize().x, GetSize().y);

    if (static_interactor.HasLock())
    {
        this->OnInput(controller);
        this->m_root_panel->Input(controller);
       
    }
     this->OnUpdate(controller);
        this->m_root_panel->Update(controller);

    this->OnRender(controller);
    this->m_root_panel->Render(controller);


    if (static_interactor.HasLock()) {
        // DrawCursor(controller);
    }

}

    void Window::DragBar::Update(Controller& controller, Window & parent, std::string title)
    {
        // WindowManager &manager = state.static_context.window_manager;
        // @brief: updates the window title and vieware position based on the dragging input
        // @returns: the new position of the parent window which will change if the title bar is being dragged
        Vector2 parent_position = parent.GetPosition();
        Vector2 parent_size = parent.GetSize();

        Input &input = controller.GetInput();
        Graphics &graphics = controller.GetGraphics();

        // Sync the position and size of the drag bar with the parent window
        SetPosition(parent_position.x, parent_position.y - GetSize().y);
        SetSize(parent_size.x, GetSize().y);

        // If the mouse is dragging in the drag bar, move the console window
        bool is_held = input.IsHeld(Mouse::LEFT);
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
        if (input.IsPressed(Mouse::LEFT) && is_inside)
        {
            // manager.RequestFocus(&parent);
            // parent.static_interactor.TryLock();
        }

        int x = parent_position.x;
        int y = parent_position.y;

        if (is_being_dragged && parent.static_interactor.HasLock())
        {
            x = input.GetMouseX() - (parent_size.x / 2);
            y = input.GetMouseY() + (GetSize().y / 2);
            x = std::clamp(x, 0, (int)(graphics.GetWidth() - parent_size.x));
            y = std::clamp(y, (int)GetSize().y, (int)(graphics.GetHeight() - (parent_size.y)));
            SetPosition(x, y - GetSize().y);
        }
        
        // Pallette &pallette = state.static_context.pallette;
        Font &font = controller.GetFontManager().GetFont("default");
        FontGraphics font_graphics(graphics, font);
        
        // Draw the title bar
        Pixel color = is_inside ? Color::GRAY : Color::DARK_GRAY;
        graphics.FillRect(color, x, y - GetSize().y, parent_size.x, GetSize().y);

        // Draw the title text centered in the title bar
        Vector2 text_size = font_graphics.MeasureString(title);
        int text_x = x + (parent_size.x / 2) - (text_size.x / 2);
        int text_y = y - GetSize().y + (GetSize().y / 2) - (text_size.y / 2);
        font_graphics.DrawString(Color::WHITE, title, text_x, text_y);
        // Draw border on top, left and right
        graphics.DrawLine(Color::WHITE, x, y - GetSize().y, x + parent_size.x, y - GetSize().y);
        graphics.DrawLine(Color::WHITE, x, y - GetSize().y, x, y);
        graphics.DrawLine(Color::WHITE, x + parent_size.x, y - GetSize().y, x + parent_size.x, y);
    };
}