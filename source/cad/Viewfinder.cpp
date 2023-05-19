#include <cad/Viewfinder.h>
#include <cad/Controller.h>

namespace Cad {

SnapViewfinder::SnapViewfinder() {}

void SnapViewfinder::Pan(int dx, int dy)
{
    pan_x += dx * scale;
    pan_y += dy * scale;
}

void SnapViewfinder::Zoom(int delta) { scale += delta * 2.0f; }

void SnapViewfinder::Zero(Core::Controller& controller)
{
    Core::Graphics& graphics = controller.GetGraphics();
    pan_x = graphics.GetWidth() / 2.0f;
    pan_y = graphics.GetHeight() / 2.0f;
    scale = 1.0f;
}

Core::Vector2 SnapViewfinder::GetCursor(Core::Controller& controller)
{
    Core::Input& input = controller.GetInput();
    Core::Vector2 cursor = input.GetMousePosition();
    Core::Transform view_transform = GetViewTransform();
    Core::Vector2 cursor_world = view_transform.Inverse().Apply(cursor);

    // Snap cursor to grid
    float grid_x = std::round(cursor_world.x / grid_size) * grid_size;
    float grid_y = std::round(cursor_world.y / grid_size) * grid_size;
    cursor_world = Core::Vector2(grid_x, grid_y);
    cursor = view_transform.Apply(cursor_world);

    return cursor;
}

void SnapViewfinder::Update(Core::Controller& controller)
{
    Core::Input& input = controller.GetInput();
    Core::Graphics& graphics = controller.GetGraphics();

    Core::Transform view_transform = GetViewTransform();
    Core::Transform inverse_transform = view_transform.Inverse();

    // Draw the grid lines
    // The idea is just draw all the pixels and then snap each one to the nearest grid line
    // This is a lot easier than trying to figure out the grid lines in screen space
    // and then drawing them directly, however; it is not very efficient and will
    // need to be optimized later, as it is very slow for screen sizes
    for (int x = 0; x < graphics.GetWidth(); x++) {
        for (int y = 0; y < graphics.GetWidth(); y++) {
            // Get the current world position
            Core::Vector2 screen_pos = Core::Vector2(x, y);
            Core::Vector2 world_pos = inverse_transform.Apply(screen_pos);

            // Snap the world position to the grid
            float grid_x = std::round(world_pos.x / grid_size) * grid_size;
            float grid_y = std::round(world_pos.y / grid_size) * grid_size;
            world_pos = Core::Vector2(grid_x, grid_y);
            // Go back to screen space and draw the pixel
            screen_pos = view_transform.Apply(world_pos);
            graphics.SetPixel(Core::Color::GRAY, screen_pos.x, screen_pos.y);
        }
    }

    // Draw the origin lines
    Core::Vector2 origin = Core::Vector2(0, 0);
    Core::Vector2 origin_screen = view_transform.Apply(origin);
    graphics.DrawLine(Core::Color::RED, origin_screen.x, 0, origin_screen.x, graphics.GetHeight());
    graphics.DrawLine(Core::Color::GREEN, 0, origin_screen.y, graphics.GetWidth(), origin_screen.y);

    // Draw the cursor as a cross hair and reticle
    Core::Vector2 screen_cursor = GetCursor(controller);
    // Draw reticle as four corners centered on the cursor
    float topleft_x = screen_cursor.x - 2;
    float topleft_y = screen_cursor.y - 2;
    float bottomright_x = screen_cursor.x + 2;
    float bottomright_y = screen_cursor.y + 2;

    graphics.DrawLine(Core::Color::WHITE, topleft_x, topleft_y, topleft_x + 1, topleft_y);
    graphics.DrawLine(Core::Color::WHITE, topleft_x, topleft_y, topleft_x, topleft_y + 1);
    graphics.DrawLine(Core::Color::WHITE, bottomright_x, bottomright_y, bottomright_x - 1, bottomright_y);
    graphics.DrawLine(Core::Color::WHITE, bottomright_x, bottomright_y, bottomright_x, bottomright_y - 1);

    graphics.DrawLine(Core::Color::WHITE, topleft_x, bottomright_y, topleft_x + 1, bottomright_y);
    graphics.DrawLine(Core::Color::WHITE, topleft_x, bottomright_y, topleft_x, bottomright_y - 1);
    graphics.DrawLine(Core::Color::WHITE, bottomright_x, topleft_y, bottomright_x - 1, topleft_y);
    graphics.DrawLine(Core::Color::WHITE, bottomright_x, topleft_y, bottomright_x, topleft_y + 1);


    // Draw the four lines of the cross hair
    graphics.DrawLine(Core::Color::WHITE, screen_cursor.x - 10, screen_cursor.y, screen_cursor.x - 3, screen_cursor.y);
    graphics.DrawLine(Core::Color::WHITE, screen_cursor.x + 3, screen_cursor.y, screen_cursor.x + 10, screen_cursor.y);
    graphics.DrawLine(Core::Color::WHITE, screen_cursor.x, screen_cursor.y - 10, screen_cursor.x, screen_cursor.y - 3);
    graphics.DrawLine(Core::Color::WHITE, screen_cursor.x, screen_cursor.y + 3, screen_cursor.x, screen_cursor.y + 10);
}

Core::Transform SnapViewfinder::GetViewTransform()
{
    Core::Transform view_transform(pan_x, pan_y, scale, 0);
    return view_transform;
}
} // namespace Cad