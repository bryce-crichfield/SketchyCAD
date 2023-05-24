#include <cad/Application.h>
#include <cad/object/ObjectVisitor.h>

namespace Cad {

struct CreateCircleHandler : public InputHandler {
    std::stack<Core::Vector2> points;
    void OnInput(Cad::Controller& controller) override
    {
        auto& input = controller.GetInput();
        auto cursor = controller.GetViewfinder().GetCursor(controller);
        auto transform = controller.GetViewfinder().GetViewTransform();
        auto cursor_world = transform.Inverse().Apply(cursor);

        if (input.IsPressed(Core::Mouse::LEFT)) {
            points.push(cursor_world);
        }

        if (input.IsPressed(Core::Key::Escape)) {
            points = std::stack<Core::Vector2>();
        }

        if (points.size() == 1) {
            auto& graphics = controller.GetGraphics();
            auto& point = points.top();
            auto length = (point - cursor_world).Length();
            graphics.PushTransform(transform);
            graphics.DrawCircle(Core::Color::GRAY, point.x, point.y, length);
            graphics.DrawLine(Core::Color::GRAY, point.x, point.y, cursor_world.x, cursor_world.y);
            graphics.PopTransform();

            Core::Font& font = controller.GetFontManager().GetFont("default");
            Core::FontGraphics font_graphics(graphics, font);
            // to string length with 2 decimal places
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << length;
            std::string text = ss.str();
            float draw_x = cursor.x + 10;
            float draw_y = cursor.y + 10;
            font_graphics.DrawString(Core::Color::WHITE, text, draw_x, draw_y);
        }

        if (points.size() == 2) {
            Core::Transform view_transform = controller.GetViewfinder().GetViewTransform();
            Core::Vector2 radius = points.top();
            points.pop();
            Core::Vector2 center = points.top();
            points.pop();
            float r = (center - radius).Length();
            auto builder = Cad::CircleObjectBuilder(center, r);
            controller.GetRegistry().CreateObject(builder);
        }
    }
};


struct DeselectAllVisitor : ObjectVisitor {
    DeselectAllVisitor() = default;

    void Visit(LineObject& object) override { object.Deselect(); }

    void Visit(CircleObject& object) override { object.Deselect(); }

    void Visit(PolylineObject& object) override { object.Deselect(); }
};

struct SelectionVisitor : public ObjectVisitor {
    Core::Vector2 topleft;
    float width;
    float height;

    SelectionVisitor(Core::Vector2 topleft, float width, float height) : topleft(topleft), width(width), height(height)
    {
    }

    void Visit(LineObject& object) override
    {
        auto& start = object.start;
        auto& end = object.end;
        if (start.x > topleft.x && start.x < topleft.x + width && start.y > topleft.y && start.y < topleft.y + height) {
            std::cout << "Selecting line" << std::endl;
            object.Select();
        }
        if (end.x > topleft.x && end.x < topleft.x + width && end.y > topleft.y && end.y < topleft.y + height) {
            std::cout << "Selecting line" << std::endl;
            object.Select();
        }
    }

    void Visit(CircleObject& object) override {
        // Check to see if the circle is within the selection box
        auto& center = object.center;
        float radius = object.radius;
        if (center.x - radius > topleft.x && center.x + radius < topleft.x + width && center.y - radius > topleft.y && center.y + radius < topleft.y + height) {
            std::cout << "Selecting circle" << std::endl;
            object.Select();
        }

    }

    void Visit(PolylineObject& object) override {}
};
struct SelectionModeHandler : public InputHandler {
    std::stack<Core::Vector2> points;

    void OnInput(Cad::Controller& controller) override
    {
        auto& input = controller.GetInput();
        auto cursor = controller.GetViewfinder().GetCursor(controller);
        auto transform = controller.GetViewfinder().GetViewTransform();

        if (input.IsPressed(Core::Key::Space)) {
            // Deselect all objects
            auto& registry = controller.GetRegistry();
            DeselectAllVisitor visitor;
            registry.VisitObjects(visitor);
            
        }

        if (points.size() == 0) {
            if (input.IsPressed(Core::Mouse::LEFT)) {
                points.push(cursor);
            }
        }
        else if (points.size() == 1) {
            // Draw the selection box
            auto& graphics = controller.GetGraphics();
            auto& point = points.top();

            Core::Vector2 topleft = point;
            if (cursor.x < point.x) {
                topleft.x = cursor.x;
            }
            if (cursor.y < point.y) {
                topleft.y = cursor.y;
            }

            Core::Vector2 bottomright = cursor;
            if (cursor.x < point.x) {
                bottomright.x = point.x;
            }
            if (cursor.y < point.y) {
                bottomright.y = point.y;
            }

            float width = bottomright.x - topleft.x;
            float height = bottomright.y - topleft.y;

            {
                // Draw the selection box
                Core::Vector2 topright = topleft + Core::Vector2(width, 0);
                Core::Vector2 bottomleft = topleft + Core::Vector2(0, height);                
                graphics.DrawDotted(Core::Color::ORANGE, topleft.x, topleft.y, topright.x, topright.y, 5);
                graphics.DrawDotted(Core::Color::ORANGE, topleft.x, topleft.y, bottomleft.x, bottomleft.y, 5);
                graphics.DrawDotted(Core::Color::ORANGE, bottomright.x, bottomright.y, topright.x, topright.y, 5);
                graphics.DrawDotted(Core::Color::ORANGE, bottomright.x, bottomright.y, bottomleft.x, bottomleft.y, 5);
            }

            if (input.IsPressed(Core::Mouse::LEFT)) {
                // Perform the selection
                auto& registry = controller.GetRegistry();
                auto topleft_world = transform.Inverse().Apply(topleft);
                float world_width = width / transform.scale;
                float world_height = height / transform.scale;
                SelectionVisitor visitor(topleft_world, world_width, world_height);
                registry.VisitObjects(visitor);
                points.pop();
            }
        }
    }
};

struct LineCreateHandler : public InputHandler {
    std::stack<Core::Vector2> points;
    float DrawArc(Core::Graphics& graphics, Core::Vector2 origin, Core::Vector2 start, Core::Vector2 end)
    {
        Core::Transform transform = graphics.GetTransform();
        origin = transform.Apply(origin);
        start = transform.Apply(start);
        end = transform.Apply(end);

        float start_radius = (start - origin).Length();
        float end_radius = (end - origin).Length();
        float radius = std::min(start_radius, end_radius);
        float start_angle = (start - origin).Angle();
        float end_angle = (end - origin).Angle();

        // sort angles
        if (start_angle > end_angle) {
            float tmp = start_angle;
            start_angle = end_angle;
            end_angle = tmp;
        }

        bool popped = false;
        if (!graphics.GetStackSize() == 0) {
            graphics.PopTransform();
            popped = true;
        }

        float angle = start_angle;
        float step = 0.01f;
        while (angle < end_angle) {
            float next_angle = angle + step;
            if (next_angle > end_angle) {
                next_angle = end_angle;
            }

            Core::Vector2 p0 = origin + Core::Vector2::FromAngle(angle) * radius;
            Core::Vector2 p1 = origin + Core::Vector2::FromAngle(next_angle) * radius;
            std::cout << "p0: " << p0.ToString() << std::endl;
            graphics.DrawLine(Core::Color::GRAY, p0.x, p0.y, p1.x, p1.y);

            angle = next_angle;
        }

        if (popped) {
            graphics.PushTransform(transform);
        }
        return std::max(start_angle, end_angle);
    }
    void OnInput(Cad::Controller& controller)
    {
        auto& input = controller.GetInput();
        auto cursor = controller.GetViewfinder().GetCursor(controller);
        auto transform = controller.GetViewfinder().GetViewTransform();
        auto cursor_world = transform.Inverse().Apply(cursor);

        if (input.IsPressed(Core::Mouse::LEFT)) {
            points.push(cursor_world);
        }

        if (input.IsPressed(Core::Key::Escape)) {
            points = std::stack<Core::Vector2>();
        }

        if (points.size() == 1) {
            auto& graphics = controller.GetGraphics();
            auto& point = points.top();
            auto length = (cursor_world - point).Length();
            graphics.PushTransform(transform);
            graphics.DrawDotted(Core::Color::WHITE, point.x, point.y, cursor_world.x, cursor_world.y, 3);
            // Draw Horizontal Basis Line

            // the arc will go from the horizontal basis line to the cursor
            Core::Vector2 horizontal_basis = Core::Vector2(point.x + cursor_world.Length(), point.y);
            float arc_angle = DrawArc(graphics, point, horizontal_basis, cursor_world);
            graphics.DrawDotted(
                Core::Color::GREEN.Darker(), point.x, point.y, horizontal_basis.x, horizontal_basis.y, 5);

            graphics.PopTransform();

            Core::Font& font = controller.GetFontManager().GetFont("default");
            Core::FontGraphics font_graphics(graphics, font);
            // to string length with 2 decimal places
            float angle_degree = arc_angle * 180 / M_PI;
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << length;
            ss << ", " << angle_degree << "°";
            std::string text = ss.str();
            float draw_x = cursor.x + 10;
            float draw_y = cursor.y + 10;
            font_graphics.DrawString(Core::Color::WHITE, text, draw_x, draw_y);
        }

        if (points.size() == 2) {
            Core::Vector2 end = points.top();
            points.pop();
            Core::Vector2 start = points.top();
            points.pop();
            auto builder = Cad::LineObjectBuilder(start, end);
            controller.GetRegistry().CreateObject(builder);
        }
    }
};

struct AreSelectedPredicate : public ObjectPredicate {
    AreSelectedPredicate() = default;
    virtual bool Match(Object& object) { return object.IsSelected(); }
};
struct RendererVisitor : public ObjectVisitor {
    Core::Graphics& graphics;
    RendererVisitor(Core::Graphics& graphics) : graphics(graphics) {}

    void Visit(LineObject& object) override
    {
        auto start = object.start;
        auto end = object.end;
        auto color = object.IsSelected() ? Core::Color::RED : Core::Color::WHITE;
        graphics.DrawLine(color, start.x, start.y, end.x, end.y);
    }

    void Visit(CircleObject& object) override
    {
        auto& center = object.center;
        auto radius = object.radius;
        auto color = object.IsSelected() ? Core::Color::RED : Core::Color::WHITE;

        graphics.DrawCircle(color, center.x, center.y, radius);
    }

    void Visit(PolylineObject& object) override {}
};

struct TranslatedRenderVisitor : public ObjectVisitor {
    Core::Vector2 delta;
    Core::Graphics& graphics;

    TranslatedRenderVisitor(Core::Vector2 delta, Core::Graphics& graphics) : delta(delta), graphics(graphics) {}

    void Visit(LineObject& object) override
    {
        std::cout << "rendering line" << std::endl;
        auto start = object.start + delta;
        auto end = object.end + delta;

        auto color = Core::Color::BROWN;
        graphics.DrawLine(color, start.x, start.y, end.x, end.y);
    }

    void Visit(CircleObject& object) override { }

    void Visit(PolylineObject& object) override {}
};

struct TranslateObjectVisitor : public ObjectVisitor {
    Core::Vector2 delta;

    TranslateObjectVisitor(Core::Vector2 delta) : delta(delta) {}

    void Visit(LineObject& object) override
    {
        object.start += delta;
        object.end += delta;
    }

    void Visit(CircleObject& object) override { object.center += delta; }

    void Visit(PolylineObject& object) override {}
};

struct TranslateModeHandler : public InputHandler {
    std::stack<Core::Vector2> points;
    TranslateModeHandler() = default;
    void OnInput(Cad::Controller& controller)
    {
        auto& input = controller.GetInput();
        auto cursor = controller.GetViewfinder().GetCursor(controller);
        auto transform = controller.GetViewfinder().GetViewTransform();
        auto cursor_world = transform.Inverse().Apply(cursor);

        if (points.empty() && input.IsPressed(Core::Mouse::LEFT)) {
            points.push(cursor_world);
        }

        else if (points.size() == 1) {
            auto delta = cursor_world - points.top();
            auto predicate = AreSelectedPredicate();
            auto selected = controller.GetRegistry().QueryObjects(predicate);
            controller.GetGraphics().PushTransform(transform);
            TranslatedRenderVisitor visitor(delta, controller.GetGraphics());
            controller.GetRegistry().VisitObjects(selected, visitor);
            controller.GetGraphics().DrawDotted(Core::Color::RED, points.top().x, points.top().y, cursor_world.x, cursor_world.y, 5);
            controller.GetGraphics().PopTransform();

            if (input.IsPressed(Core::Mouse::LEFT)) {
                points.pop();
                TranslateObjectVisitor visitor(delta);
                controller.GetRegistry().VisitObjects(selected, visitor);
            }
        }
        

    }
};


Application::Application(Core::Gui::InteractionLock& lock) : focus(lock)
{
    registry = std::make_unique<ObjectRegistry>();
    viewfinder = std::make_unique<Viewfinder>();
    input_handler = std::make_unique<SelectionModeHandler>();
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


struct CopyVisitor : ObjectVisitor {
    Cad::ObjectRegistry& registry;
    Core::Vector2 delta;

    CopyVisitor(Cad::ObjectRegistry& registry, Core::Vector2 delta) : registry(registry), delta(delta) {}

    void Visit(LineObject& object) override
    {
        auto start = object.start + delta;
        auto end = object.end + delta;
        auto builder = Cad::LineObjectBuilder(start, end);
        registry.CreateObject(builder);
    }

    void Visit(CircleObject& object) override
    {
        auto center = object.center + delta;
        auto builder = Cad::CircleObjectBuilder(center, object.radius);
        registry.CreateObject(builder);
    }

    void Visit(PolylineObject& object) override {}
};

struct CopyInputHandler : public InputHandler {
    std::stack<Core::Vector2> points;
    CopyInputHandler() = default;
    void OnInput(Cad::Controller& controller)
    {
        auto& input = controller.GetInput();
        auto cursor = controller.GetViewfinder().GetCursor(controller);
        auto transform = controller.GetViewfinder().GetViewTransform();
        auto cursor_world = transform.Inverse().Apply(cursor);

        if (input.IsPressed(Core::Mouse::LEFT) && points.empty()) {
            points.push(cursor_world);
        }
        else if (points.size() == 1) {
            auto delta = cursor_world - points.top();
            auto predicate = AreSelectedPredicate();
            auto selected = controller.GetRegistry().QueryObjects(predicate);
            controller.GetGraphics().PushTransform(transform);
            TranslatedRenderVisitor visitor(delta, controller.GetGraphics());
            controller.GetRegistry().VisitObjects(selected, visitor);
            controller.GetGraphics().DrawDotted(Core::Color::RED, points.top().x, points.top().y, cursor_world.x, cursor_world.y, 5);
            controller.GetGraphics().PopTransform();

            if (input.IsPressed(Core::Mouse::LEFT)) {
                points.pop();
                CopyVisitor visitor(controller.GetRegistry(), delta);
                controller.GetRegistry().VisitObjects(selected, visitor);
            }
        }

    }
};

void Application::OnInput(Controller& controller)
{
    if (input_handler != nullptr) {
        input_handler->OnInput(controller);
    }

    auto& input = controller.GetInput();

    if (input.IsHeld(Core::Key::LControl) && input.IsPressed(Core::Key::A)) {
        DeselectAllVisitor visitor;
        controller.GetRegistry().VisitObjects(visitor);
        
    }

    if (input.IsPressed(Core::Key::L)) {
        input_handler = std::make_unique<LineCreateHandler>();
    }

    if (input.IsPressed(Core::Key::S)) {
        input_handler = std::make_unique<SelectionModeHandler>();
    }

    if (input.IsPressed(Core::Key::T)) {
        input_handler = std::make_unique<TranslateModeHandler>();
    }

    if (input.IsPressed(Core::Key::C)) {
        input_handler = std::make_unique<CopyInputHandler>();
    }

    if (input.IsPressed(Core::Key::E)) {
        input_handler = std::make_unique<CreateCircleHandler>();
    }

    if (input.IsPressed(Core::Key::Delete)) {
        AreSelectedPredicate predicate;
        auto selected = controller.GetRegistry().QueryObjects(predicate);
        controller.GetRegistry().DeleteObjects(selected);
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