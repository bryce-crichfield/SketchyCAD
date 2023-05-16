#include <Cad.h>

float Cad::Cursor::GetX()
{
    return x;
}

float Cad::Cursor::GetY()
{
    return y;
}

Vector2 Cad::Cursor::GetPosition()
{
    return Vector2(x, y);
}

void Cad::Cursor::OnInput(Controller &controller)
{
    x = controller.GetInput().GetMouseX();
    y = controller.GetInput().GetMouseY();
}

void Cad::Cursor::OnUpdate(Controller &controller)
{
    if (!snap_mode) {
        return;
    }

    Camera &camera = controller.GetCamera();
    Vector2 screen_mouse = Vector2(x, y);
    Vector2 world_mouse = camera.ScreenToWorld(screen_mouse);
    // Snap to grid
    float grid_size = controller.GetGridSize();
    float grid_x = round(world_mouse.x / grid_size) * grid_size;
    float grid_y = round(world_mouse.y / grid_size) * grid_size;
    // Convert back to screen coordinates
    Vector2 grid_mouse = Vector2(grid_x, grid_y);
    Vector2 rounded_mouse = camera.WorldToScreen(grid_mouse);

    // Snap to grid 
    x = rounded_mouse.x;
    y = rounded_mouse.y;
}

void Cad::Cursor::OnRender(Controller &controller)
{
    // Draw Cursor with Reticle
    Input &input = controller.GetInput();
    Graphics &graphics = controller.GetGraphics();

    float cursor_x = GetX();
    float cursor_y = GetY();

    float box_w = 5;
    float box_h = 5;
    float box_dx = ceil(box_w / 2);
    float box_dy = ceil(box_h / 2);
    float box_x = ceil(cursor_x - box_dx);
    float box_y = ceil(cursor_y - box_dy);

    graphics.DrawRect(Color::WHITE, box_x, box_y, box_w, box_h);
    graphics.DrawLine(Color::WHITE, cursor_x, 0, cursor_x, cursor_y - box_dy);
    graphics.DrawLine(Color::WHITE, cursor_x, cursor_y + box_dy, cursor_x, graphics.GetHeight());
    graphics.DrawLine(Color::WHITE, 0, cursor_y, cursor_x - box_dx, cursor_y);
    graphics.DrawLine(Color::WHITE, cursor_x + box_dx, cursor_y, graphics.GetWidth(), cursor_y);

    // Draw Coordinate Indicator
    Font &font = controller.GetStaticContext().fonts.GetFont("default");
    FontGraphics font_graphics = FontGraphics(graphics, font);
    font_graphics.SetDisplayWidth(8);
    Vector2 screen_mouse = Vector2(cursor_x, cursor_y);
    Vector2 world_mouse = controller.GetCamera().ScreenToWorld(screen_mouse);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << "(" << world_mouse.x << ", " << world_mouse.y << ")";
    std::string text = ss.str();
    font_graphics.DrawString(Color::WHITE, text, screen_mouse.x + 3, screen_mouse.y + 3);
}
