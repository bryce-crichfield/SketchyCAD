#include <cad/Viewfinder.h>
#include <cad/Controller.h>
#include <core/Core.h>

namespace Cad {

Viewfinder::Viewfinder() : ray(Core::Vector2(0, 0), Core::Vector2(2, 1)) {
    cursor = std::make_unique<Cursor>();
}

void Viewfinder::Pan(int dx, int dy)
{
    pan_x += dx * scale;
    pan_y += dy * scale;
}

void Viewfinder::Zoom(int delta, Core::Vector2 world_center) { 
    scale += delta * 2.0f; 
    pan_x -= world_center.x * delta * 2.0f;
    pan_y -= world_center.y * delta * 2.0f;
}

void Viewfinder::Zero(Core::Controller& controller)
{
    Core::Graphics& graphics = controller.GetGraphics();
    pan_x = graphics.GetWidth() / 2.0f;
    pan_y = graphics.GetHeight() / 2.0f;
    scale = 1.0f;
}

Core::Vector2 Viewfinder::GetCursor(Core::Controller& controller)
{
    auto vector = cursor->GetScreenPosition();
    return vector;
}

void Viewfinder::Update(Core::Controller& controller, ObjectRegistry& registry, RayBank& ray_bank)
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

    // If the mouse is scrolling then zoom in or out
    // also adjust the zoom based on the mouse position
    // so it appears to zoom in on the mouse position
    if (input.GetScrollDeltaY() != 0) {
        auto cursor_screen = GetCursor(controller);
        auto cursor_world = inverse_transform.Apply(cursor_screen);
        Zoom(input.GetScrollDeltaY(), cursor_world);
    }

    // Do mouse dragging
    if (input.IsHeld(Core::Mouse::MIDDLE)) {
        auto delta_mouse = controller.GetInput().GetMouseDelta();        
        pan_x += delta_mouse.x;
        pan_y += delta_mouse.y;
    }

    // Draw the cursor as a cross hair and reticle
    Core::Vector2 screen_cursor = GetCursor(controller);

    if (input.IsHeld(Core::Key::LShift)) {
        cursor->SetGridSnapped(true);
    } else {
        cursor->SetGridSnapped(false);
    }
    // draw the raycast line
    // controller.GetGraphics().DrawLine(Core::Color::WHITE, ray.x, ray.y, ray.x + ray.dx * 1000, ray.y + ray.dy * 1000);
    cursor->Update(controller, registry, view_transform, grid_size, ray_bank);

}

Core::Transform Viewfinder::GetViewTransform()
{
    Core::Transform view_transform(pan_x, pan_y, scale, 0);
    return view_transform;
}
} // namespace Cad