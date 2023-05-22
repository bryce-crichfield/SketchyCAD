#include <cad/Application.h>
#include <cad/object/ObjectVisitor.h>

namespace Cad {
struct RendererVisitor : public ObjectVisitor {
    Core::Graphics& graphics;
    RendererVisitor(Core::Graphics& graphics) : graphics(graphics) {}

    void Visit(LineObject& object) override
    {
        auto start = object.start;
        auto end = object.end;
        graphics.DrawLine(Core::Color::WHITE, start.x, start.y, end.x, end.y);
    }

    void Visit(CircleObject& object) override
    {
        auto& center = object.center;
        auto radius = object.radius;
        graphics.DrawCircle(Core::Color::WHITE, center.x, center.y, radius);
    }

    void Visit(PolylineObject& object) override {}
};

Application::Application(Core::Gui::InteractionLock& lock)
    : focus(lock)
{
    registry = std::make_unique<ObjectRegistry>();
    viewfinder = std::make_unique<SnapViewfinder>();
    input_handler = std::make_unique<LineCreateHandler>();
    dispatcher = std::make_shared<Dispatcher>();
}

void Application::OnStart(Controller& controller)
{
    viewfinder->Zero(controller);
    viewfinder->Pan(0, 0);

    CircleObjectBuilder circle(Core::Vector2(0, 0), 1);
    registry->CreateObject(circle);

    LineObjectBuilder line(Core::Vector2(2, 2), Core::Vector2(4, 1));
    registry->CreateObject(line);
}

void Application::OnInput(Controller& controller) {
    if (input_handler != nullptr) {
        input_handler->OnInput(controller);
    }
}

void Application::OnUpdate(Controller& controller) {}

void Application::OnRender(Controller& controller)
{
    controller.GetViewfinder().Update(controller, *registry);
    Core::Transform view_transform = controller.GetViewfinder().GetViewTransform();
    controller.GetGraphics().PushTransform(view_transform);
    RendererVisitor renderer(controller.GetGraphics());
    controller.GetRegistry().VisitObjects(renderer);
    controller.GetGraphics().PopTransform();
}

void Application::Update(Controller& controller)
{
    dispatcher->Execute(controller);

    if (focus.TryLock()) {
        OnInput(controller);
        OnUpdate(controller);
    }

    OnRender(controller);
}
} // namespace Cad