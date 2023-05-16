#include <Cad.h>

namespace Cad::Object {
Line::Line(unsigned id, Vector2 start, Vector2 end) : start(start), end(end), IObject(id) {}

void Line::Update(Controller& controller)
{
    bool is_selected = controller.GetClipboard().IsSelected(id);
    Pixel color = is_selected ? Color::RED : Color::WHITE;
    controller.GetGraphics().DrawLine(color, start.x, start.y, end.x, end.y);
}

bool Line::IsHovered(Controller& controller)
{
    // mouse pick the line segment with some tolerance
    Vector2 screen_mouse = controller.GetCursor().GetPosition();
    Vector2 world_mouse = controller.GetCamera().ScreenToWorld(screen_mouse);
    Vector2 closest_point = world_mouse.ClosestPointOnLine(start, end);
    float distance = (world_mouse - closest_point).Length();
    return distance < 2;
}

Circle::Circle(unsigned id, Vector2 center, float radius) : center(center), radius(radius), IObject(id) {}

void Circle::Update(Controller& controller)
{
    bool is_selected = controller.GetClipboard().IsSelected(id);
    Pixel color = is_selected ? Color::RED : Color::WHITE;
    controller.GetGraphics().DrawCircle(color, center.x, center.y, radius);
}

bool Circle::IsHovered(Controller& controller)
{
    // Determine if the mouse is within the circle
    Vector2 screen_mouse = controller.GetCursor().GetPosition();
    Vector2 world_mouse = controller.GetCamera().ScreenToWorld(screen_mouse);
    bool is_hovered = (world_mouse - center).Length() < radius;
    return (world_mouse - center).Length() < radius;
}

} // namespace Cad::Object
