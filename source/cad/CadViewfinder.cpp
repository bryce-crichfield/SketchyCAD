#include <cad/CadViewfinder.h>

namespace Cad {

SnapViewfinder::SnapViewfinder() {}

void SnapViewfinder::Pan(int dx, int dy)
{
    pan_x += dx * scale;
    pan_y += dy * scale;
}

void SnapViewfinder::Zoom(int delta) { scale += delta * 2.0f; }

void SnapViewfinder::Zero(Graphics& graphics)
{
    pan_x = graphics.GetWidth() / 2.0f;
    pan_y = graphics.GetHeight() / 2.0f;
}

Vector2 SnapViewfinder::GetCursor(Input& input)
{
    Vector2 cursor = input.GetMousePosition();
    Transform view_transform = GetViewTransform();
    Vector2 cursor_world = view_transform.Inverse().Apply(cursor);

    // Snap cursor to grid
    float grid_x = std::round(cursor_world.x / grid_size) * grid_size;
    float grid_y = std::round(cursor_world.y / grid_size) * grid_size;
    cursor_world = Vector2(grid_x, grid_y);
    cursor = view_transform.Apply(cursor_world);

    return cursor;
}

void SnapViewfinder::OnRender(Graphics& graphics)
{
    Transform view_transform = GetViewTransform();
    Transform inverse_transform = view_transform.Inverse();

    // Draw the origin lines
    Vector2 origin = Vector2(0, 0);
    Vector2 origin_screen = view_transform.Apply(origin);
    graphics.DrawLine(Color::RED, origin_screen.x, 0, origin_screen.x, graphics.GetHeight());
    graphics.DrawLine(Color::GREEN, 0, origin_screen.y, graphics.GetWidth(), origin_screen.y);

    // Draw the grid lines
    // The idea is just draw all the pixels and then snap each one to the nearest grid line
    // This is a lot easier than trying to figure out the grid lines in screen space
    // and then drawing them directly, however; it is not very efficient and will
    // need to be optimized later, as it is very slow for screen sizes
    for (int x = 0; x < graphics.GetWidth(); x++) {
        for (int y = 0; y < graphics.GetWidth(); y++) {
            // Get the current world position
            Vector2 screen_pos = Vector2(x, y);
            Vector2 world_pos = inverse_transform.Apply(screen_pos);

            // Snap the world position to the grid
            float grid_x = std::round(world_pos.x / grid_size) * grid_size;
            float grid_y = std::round(world_pos.y / grid_size) * grid_size;
            world_pos = Vector2(grid_x, grid_y);
            // Go back to screen space and draw the pixel
            screen_pos = view_transform.Apply(world_pos);
            graphics.SetPixel(Color::GRAY, screen_pos.x, screen_pos.y);
        }
    }
}

Transform SnapViewfinder::GetViewTransform()
{
    Transform view_transform(pan_x, pan_y, scale, 0);
    return view_transform;
}
} // namespace Cad