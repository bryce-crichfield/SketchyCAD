#include <cad/CadObject.h>

namespace Cad
{

/* ------------------------------------------------------------------------------------------------------------------ */
/*                                                        Line                                                        */
/* ------------------------------------------------------------------------------------------------------------------ */

LineObject::LineObject(Vector2 start, Vector2 end) : start(start), end(end) {}

void LineObject::Accept(ObjectVisitor& visitor) { visitor.Visit(*this); }

std::unique_ptr<Object> LineObject::Clone() const
{
    auto clone = std::make_unique<LineObject>(start, end);
    return clone;
}

std::string LineObject::ToString() const 
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Line: " << start.x << ", " << start.y << " -> " << end.x << ", " << end.y;
    return ss.str();
}

/* ------------------------------------------------------------------------------------------------------------------ */
/*                                                       Circle                                                       */
/* ------------------------------------------------------------------------------------------------------------------ */

CircleObject::CircleObject(Vector2 center, float radius) : center(center), radius(radius) {}

void CircleObject::Accept(ObjectVisitor& visitor) { visitor.Visit(*this); }

std::unique_ptr<Object> CircleObject::Clone() const 
{
    auto clone = std::make_unique<CircleObject>(center, radius);
    return clone;
}

std::string CircleObject::ToString() const 
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Circle: " << center.x << ", " << center.y << " -> " << radius;
    return ss.str();
}

/* ------------------------------------------------------------------------------------------------------------------ */
/*                                                      Polyline                                                      */
/* ------------------------------------------------------------------------------------------------------------------ */

PolylineObject::PolylineObject(std::vector<Vector2> points) : points(points) {}

void PolylineObject::Accept(ObjectVisitor& visitor) { visitor.Visit(*this); }

std::unique_ptr<Object> PolylineObject::Clone() const 
{
    auto clone = std::make_unique<PolylineObject>(points);
    return clone;
}

std::string PolylineObject::ToString() const 
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Polyline: ";
    for (auto& point : points) {
        ss << point.x << ", " << point.y << " -> ";
    }
    return ss.str();
}

} // namespace Cad::Core

// void Line::Update(Controller& controller)
// {
//     bool is_selected = controller.GetClipboard().IsSelected(id);
//     Pixel color = is_selected ? Color::RED : Color::WHITE;
//     controller.GetGraphics().DrawLine(color, start.x, start.y, end.x, end.y);
// }

// bool Line::IsHovered(Controller& controller)
// {
//     // mouse pick the line segment with some tolerance
//     Vector2 screen_mouse = controller.GetCursor().GetPosition();
//     Vector2 world_mouse = controller.GetCamera().ScreenToWorld(screen_mouse);
//     Vector2 closest_point = world_mouse.ClosestPointOnLine(start, end);
//     float distance = (world_mouse - closest_point).Length();
//     return distance < 2;
// }

// void Circle::Update(Controller& controller)
// {
//     bool is_selected = controller.GetClipboard().IsSelected(id);
//     Pixel color = is_selected ? Color::RED : Color::WHITE;
//     controller.GetGraphics().DrawCircle(color, center.x, center.y, radius);
// }

// bool Circle::IsHovered(Controller& controller)
// {
//     // Determine if the mouse is within the circle
//     Vector2 screen_mouse = controller.GetCursor().GetPosition();
//     Vector2 world_mouse = controller.GetCamera().ScreenToWorld(screen_mouse);
//     bool is_hovered = (world_mouse - center).Length() < radius;
//     return (world_mouse - center).Length() < radius;
// }