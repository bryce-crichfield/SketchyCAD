#include <Cad.h>

namespace Cad::Mode
{

void Line::Update(Controller &controller)
{
    Input &input = controller.GetInput();
    if (points.size() == 2)
    {
        Vector2 start = points.top();
        points.pop();
        Vector2 end = points.top();
        points.pop();
        auto command = std::make_unique<Command::CreateLine>(start, end);
        controller.Command(std::move(command));
    }

    if (input.IsPressed(MouseButton::LEFT))
    {
        Vector2 screen_mouse = controller.GetCursor().GetPosition();
        Vector2 world_mouse = controller.GetCamera().ScreenToWorld(screen_mouse);
        points.push(world_mouse);
    }

    if (points.size() == 1)
    {
        Vector2 screen_mouse = controller.GetCursor().GetPosition();
        Vector2 world_mouse = controller.GetCamera().ScreenToWorld(screen_mouse);
        Vector2 center = points.top();

        Graphics &graphics = controller.GetGraphics();
        graphics.PushTransform(controller.GetCamera().GetTransform());
        graphics.DrawLine(Color::GRAY, center.x, center.y, world_mouse.x, world_mouse.y);
        graphics.PopTransform();
    }
}

void Circle::Update(Controller &controller)
{
    auto &input = controller.GetInput();
    if (points.size() == 2)
    {
        Vector2 radius = points.top();
        points.pop();
        Vector2 center = points.top();
        points.pop();

        auto command = std::make_unique<Command::CreateCircle>(center, radius);
        controller.Command(std::move(command));
    }

    if (input.IsPressed(MouseButton::LEFT))
    {
        Vector2 screen_mouse = controller.GetCursor().GetPosition();
        Vector2 world_mouse = controller.GetCamera().ScreenToWorld(screen_mouse);
        points.push(world_mouse);
    }

    if (points.size() == 1)
    {
        Vector2 screen_mouse = controller.GetCursor().GetPosition();
        Vector2 world_mouse = controller.GetCamera().ScreenToWorld(screen_mouse);
        Vector2 center = points.top();

        Graphics &graphics = controller.GetGraphics();
        graphics.PushTransform(controller.GetCamera().GetTransform());
        graphics.DrawLine(Color::GRAY, center.x, center.y, world_mouse.x, world_mouse.y);
        graphics.DrawCircle(Color::GRAY, center.x, center.y, (center - world_mouse).Length());
        graphics.PopTransform();
    }
}
} // namespace Cad::Mode