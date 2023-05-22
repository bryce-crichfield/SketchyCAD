#pragma once

#include <core/Core.h>
#include <core/gui/InteractionLock.h>
#include <core/gui/Interactor.h>

#include <cad/Controller.h>
#include <cad/Viewfinder.h>
#include <cad/object/ObjectRegistry.h>
#include <cad/Dispatcher.h>

#include <iomanip>

namespace Cad {

class InputHandler {
  public:
    virtual ~InputHandler() = default;
    virtual void OnInput(Cad::Controller& controller) = 0;
};

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

struct LineCreateHandler : public InputHandler {
    std::stack<Core::Vector2> points;
    void DrawArc(Core::Graphics& graphics, Core::Vector2 origin, Core::Vector2 start, Core::Vector2 end) {
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
            auto length = (cursor_world - point ).Length();
            graphics.PushTransform(transform);
            graphics.DrawDotted(Core::Color::GRAY, point.x, point.y, cursor_world.x, cursor_world.y, 1);
            // Draw Horizontal Basis Line
            
            // the arc will go from the horizontal basis line to the cursor
            Core::Vector2 horizontal_basis = Core::Vector2(point.x + cursor_world.Length(), point.y);
            DrawArc(graphics, point, horizontal_basis, cursor_world);            
            graphics.DrawDotted(Core::Color::GREEN, point.x, point.y, horizontal_basis.x, horizontal_basis.y, 3);

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
            Core::Vector2 end = points.top();
            points.pop();
            Core::Vector2 start = points.top();
            points.pop();
            auto builder = Cad::LineObjectBuilder(start, end);
            controller.GetRegistry().CreateObject(builder);
        }
    }
};

class Application {
    Core::Gui::Interactor focus;
    std::unique_ptr<ObjectRegistry> registry;
    std::unique_ptr<Viewfinder> viewfinder;
    std::unique_ptr<InputHandler> input_handler;
    std::shared_ptr<Dispatcher> dispatcher;

  public:
    Application(Core::Gui::InteractionLock& lock);

    void OnStart(Controller& controller);
    void Update(Controller& controller);
    void OnInput(Controller& controller);
    void OnUpdate(Controller& controller);
    void OnRender(Controller& controller);

    Cad::Controller CreateController(Core::Controller& controller)
    {
        return Cad::Controller(controller, *registry, *viewfinder);
    }

    std::shared_ptr<Dispatcher>& GetDispatcher() { return dispatcher; }
};

} // namespace Cad