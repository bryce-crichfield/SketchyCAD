#include <cad/CadApplication.h>

namespace Cad {
struct RendererVisitor : public ObjectVisitor {
    Graphics& graphics;
    RendererVisitor(Graphics& graphics) : graphics(graphics) {}

    void Visit(LineObject& object) override
    {
        auto start = object.start;
        auto end = object.end;
        graphics.DrawLine(Color::WHITE, start.x, start.y, end.x, end.y);
    }

    void Visit(CircleObject& object) override
    {
        auto& center = object.center;
        auto radius = object.radius;
        graphics.DrawCircle(Color::WHITE, center.x, center.y, radius);
    }

    void Visit(PolylineObject& object) override {}
};

Application::Application(InteractionLock& lock) : focus(lock)
{
    registry = std::make_unique<Registry>();
    viewfinder = std::make_unique<SnapViewfinder>();
}

void Application::OnStart(State& state)
{
    this->state = &state;

    viewfinder->Zero(state.graphics);
    viewfinder->Pan(0, 0);

    CircleObjectBuilder circle(Vector2(0, 0), 1);
    registry->CreateObject(circle);

    LineObjectBuilder line(Vector2(0, 0), Vector2(1, 1));
    registry->CreateObject(line);
}

void Application::OnInput(State state)
{
    if (state.input.IsPressed(Key::W)) viewfinder->Pan(0, -1);
    if (state.input.IsPressed(Key::S)) viewfinder->Pan(0, 1);
    if (state.input.IsPressed(Key::A)) viewfinder->Pan(-1, 0);
    if (state.input.IsPressed(Key::D)) viewfinder->Pan(1, 0);
    if (state.input.IsPressed(Key::Q)) viewfinder->Zoom(1);
    if (state.input.IsPressed(Key::E)) viewfinder->Zoom(-1);
}

void Application::OnUpdate(State state)
{
    // Draw the cursor as a cross hair and reticle
    Vector2 screen_cursor = GetViewfinder().GetCursor(state.input);
    // Draw reticle as four corners centered on the cursor
    state.graphics.DrawRect(Color::WHITE, screen_cursor.x - 2, screen_cursor.y - 2, 5, 5);
    // Draw the four lines of the cross hair
    state.graphics.DrawLine(Color::WHITE, screen_cursor.x - 10, screen_cursor.y, screen_cursor.x - 2, screen_cursor.y);
    state.graphics.DrawLine(Color::WHITE, screen_cursor.x + 2, screen_cursor.y, screen_cursor.x + 10, screen_cursor.y);
    state.graphics.DrawLine(Color::WHITE, screen_cursor.x, screen_cursor.y - 10, screen_cursor.x, screen_cursor.y - 2);
    state.graphics.DrawLine(Color::WHITE, screen_cursor.x, screen_cursor.y + 2, screen_cursor.x, screen_cursor.y + 10);
}

void Application::OnRender(State state)
{
    GetViewfinder().OnRender(state.graphics);

    Transform view_transform = GetViewfinder().GetViewTransform();
    state.graphics.PushTransform(view_transform);
    RendererVisitor renderer(state.graphics);
    GetRegistry().VisitObjects(renderer);
    state.graphics.PopTransform();
}

void Application::Update(State& state)
{
    this->state = &state;

    if (focus.TryLock()) {
        OnInput(state);
        OnUpdate(state);
    }

    OnRender(state);
}

Graphics& Application::GetGraphics()
{
    if (!state) throw std::runtime_error("State is null");
    return state->graphics;
}

Input& Application::GetInput()
{
    if (!state) throw std::runtime_error("State is null");
    return state->input;
}

Chronometer& Application::GetChronometer()
{
    if (!state) throw std::runtime_error("State is null");
    return state->chrono;
}

StaticContext& Application::GetStaticContext()
{
    if (!state) throw std::runtime_error("State is null");
    return state->static_context;
}

Registry& Application::GetRegistry()
{
    if (!registry) throw std::runtime_error("Registry is null");
    return *registry;
}

Viewfinder& Application::GetViewfinder()
{
    if (!viewfinder) throw std::runtime_error("Viewfinder is null");
    return *viewfinder;
}
} // namespace Cad