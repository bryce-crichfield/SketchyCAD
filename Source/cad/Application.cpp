#include <cad/Application.h>
#include <cad/gui/Gui.h>
#include <cad/object/ObjectVisitor.h>
#include <core/graphics/ImageGraphics.h>
namespace Cad {

struct CreateCircleHandler : public InputHandler {
    std::stack<Core::Vector2> points;
    void OnInput(Cad::Controller& controller) override {
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

    SelectionVisitor(Core::Vector2 topleft, float width, float height)
        : topleft(topleft), width(width), height(height) {}

    void Visit(LineObject& object) override {
        auto& start = object.start;
        auto& end = object.end;
        if (start.x > topleft.x && start.x < topleft.x + width && start.y > topleft.y && start.y < topleft.y + height) {
            object.Select();
        }
        if (end.x > topleft.x && end.x < topleft.x + width && end.y > topleft.y && end.y < topleft.y + height) {
            object.Select();
        }
    }

    void Visit(CircleObject& object) override {
        // Check to see if the circle is within the selection box
        auto& center = object.center;
        float radius = object.radius;
        if (center.x - radius > topleft.x && center.x + radius < topleft.x + width && center.y - radius > topleft.y &&
            center.y + radius < topleft.y + height) {
            object.Select();
        }
    }

    void Visit(PolylineObject& object) override {}
};
struct SelectionModeHandler : public InputHandler {
    std::stack<Core::Vector2> points;

    void OnInput(Cad::Controller& controller) override {
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
        } else if (points.size() == 1) {
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
    std::vector<Ray> rays;

    void OnInput(Cad::Controller& controller) {
        auto& input = controller.GetInput();
        auto cursor = controller.GetViewfinder().GetCursor(controller);
        auto transform = controller.GetViewfinder().GetViewTransform();
        auto cursor_world = transform.Inverse().Apply(cursor);

        if (input.IsPressed(Core::Mouse::LEFT)) {
            points.push(cursor_world);
        }

        if (input.IsPressed(Core::Key::Escape) || input.IsPressed(Core::Key::Space)) {
            points = std::stack<Core::Vector2>();
            rays = std::vector<Ray>();

        }

        if (input.IsPressed(Core::Key::Up)) {
            rays.push_back(Ray(cursor_world, Core::Vector2(0, 1), Core::Color::RED.Darker()));
        }

        if (input.IsPressed(Core::Key::Left)) {
            rays.push_back(Ray(cursor_world, Core::Vector2(1, 0), Core::Color::GREEN.Darker()));
        }

        if (input.IsPressed(Core::Mouse::RIGHT)) {
            if (points.size() == 1) {
                points.pop();
            }
            points.push(cursor_world);
            return;
        }

        if (points.size() == 1) {
            auto& graphics = controller.GetGraphics();
            auto& point = points.top();
            auto length = (cursor_world - point).Length();

            // add any rays we may have
            for (auto& ray : rays) {
                controller.GetRayBank().AddRay(ray);
            }
            controller.GetRayBank().AddRay(point, Core::Vector2(0, 1), Core::Color::RED.Darker());
            controller.GetRayBank().AddRay(point, Core::Vector2(1, 0), Core::Color::GREEN.Darker());

            graphics.PushTransform(transform);
            graphics.DrawDotted(Core::Color::WHITE, point.x, point.y, cursor_world.x, cursor_world.y, 3);
            // Draw Horizontal Basis Line
            // the arc will go from the horizontal basis line to the cursor
            float angle = (cursor_world - point).Angle();
            graphics.DrawArc(Core::Color::WHITE, point.x, point.y, length, 0, angle);
            graphics.PopTransform();

            Core::Font& font = controller.GetFontManager().GetFont("default");
            Core::FontGraphics font_graphics(graphics, font);
            float angle_degree = angle * 180 / M_PI;
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << "L: " << length;
            ss << ", deg:" << angle_degree;
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
            rays = std::vector<Ray>();
        }
    }
};

struct AreSelectedPredicate : public ObjectPredicate {
    AreSelectedPredicate() = default;
    virtual bool Match(Object& object) { return object.IsSelected(); }
};

struct TranslatedRenderVisitor : public ObjectVisitor {
    Core::Vector2 delta;
    Core::Graphics& graphics;

    TranslatedRenderVisitor(Core::Vector2 delta, Core::Graphics& graphics) : delta(delta), graphics(graphics) {}

    void Visit(LineObject& object) override {
        auto start = object.start + delta;
        auto end = object.end + delta;

        auto color = Core::Color::BROWN;
        graphics.DrawLine(color, start.x, start.y, end.x, end.y);
    }

    void Visit(CircleObject& object) override {
        auto center = object.center + delta;
        auto radius = object.radius;
        auto color = Core::Color::BROWN;
        graphics.DrawCircle(color, center.x, center.y, radius);
    }

    void Visit(PolylineObject& object) override {}
};

struct TranslateObjectVisitor : public ObjectVisitor {
    Core::Vector2 delta;

    TranslateObjectVisitor(Core::Vector2 delta) : delta(delta) {}

    void Visit(LineObject& object) override {
        object.start += delta;
        object.end += delta;
    }

    void Visit(CircleObject& object) override { object.center += delta; }

    void Visit(PolylineObject& object) override {}
};

struct TranslateModeHandler : public InputHandler {
    std::stack<Core::Vector2> points;
    TranslateModeHandler() = default;
    void OnInput(Cad::Controller& controller) {
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
            controller.GetGraphics().DrawDotted(
                Core::Color::RED, points.top().x, points.top().y, cursor_world.x, cursor_world.y, 5);
            controller.GetGraphics().PopTransform();

            if (input.IsPressed(Core::Mouse::LEFT)) {
                points.pop();
                TranslateObjectVisitor visitor(delta);
                controller.GetRegistry().VisitObjects(selected, visitor);
            }
        }
    }
};

void Application::OnStart(Controller& controller) {
    editor.OnStart(controller);

    viewfinder->Zero(controller);
    viewfinder->Pan(0, 0);
    viewfinder->Zoom(10);
}

struct CopyVisitor : ObjectVisitor {
    Cad::ObjectRegistry& registry;
    Core::Vector2 delta;

    CopyVisitor(Cad::ObjectRegistry& registry, Core::Vector2 delta) : registry(registry), delta(delta) {}

    void Visit(LineObject& object) override {
        auto start = object.start + delta;
        auto end = object.end + delta;
        auto builder = Cad::LineObjectBuilder(start, end);
        registry.CreateObject(builder);
    }

    void Visit(CircleObject& object) override {
        auto center = object.center + delta;
        auto builder = Cad::CircleObjectBuilder(center, object.radius);
        registry.CreateObject(builder);
    }

    void Visit(PolylineObject& object) override {}
};

struct CopyInputHandler : public InputHandler {
    std::stack<Core::Vector2> points;
    CopyInputHandler() = default;
    void OnInput(Cad::Controller& controller) {
        auto& input = controller.GetInput();
        auto cursor = controller.GetViewfinder().GetCursor(controller);
        auto transform = controller.GetViewfinder().GetViewTransform();
        auto cursor_world = transform.Inverse().Apply(cursor);

        if (input.IsPressed(Core::Mouse::LEFT) && points.empty()) {
            points.push(cursor_world);
        } else if (points.size() == 1) {
            auto delta = cursor_world - points.top();
            auto predicate = AreSelectedPredicate();
            auto selected = controller.GetRegistry().QueryObjects(predicate);
            controller.GetGraphics().PushTransform(transform);
            TranslatedRenderVisitor visitor(delta, controller.GetGraphics());
            controller.GetRegistry().VisitObjects(selected, visitor);
            controller.GetGraphics().DrawDotted(
                Core::Color::RED, points.top().x, points.top().y, cursor_world.x, cursor_world.y, 5);
            controller.GetGraphics().PopTransform();

            if (input.IsPressed(Core::Mouse::LEFT)) {
                points.pop();
                CopyVisitor visitor(controller.GetRegistry(), delta);
                controller.GetRegistry().VisitObjects(selected, visitor);
            }
        }
    }
};

Editor::Editor() {
    handlers.push_back(std::make_unique<EditorHandler>(*this));
    input_handler = std::make_unique<NoOpInputHandler>();

    is_focusable = true;
}

void Editor::OnUpdate(Cad::Controller& controller) {
    if (NotFocused()) return;

    if (input_handler != nullptr) {
        if (idle) {
            idle = false;
        } else {
            input_handler->OnInput(controller);
        }
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

    if (input.IsPressed(Core::Key::Escape)) {
        input_handler = std::make_unique<NoOpInputHandler>();

        DeselectAllVisitor visitor;
        controller.GetRegistry().VisitObjects(visitor);
    }

    auto& registry = controller.GetRegistry();
    auto& ray_bank = controller.GetRayBank();
    controller.GetViewfinder().Update(controller, registry, ray_bank);
}

void Editor::OnRender(Cad::Controller& controller) {
    auto& registry = controller.GetRegistry();
    auto& ray_bank = controller.GetRayBank();
    controller.GetRayBank().AddRay(Core::Vector2(0, 0), Core::Vector2(0, 1), Core::Color::RED);
    controller.GetRayBank().AddRay(Core::Vector2(0, 0), Core::Vector2(1, 0), Core::Color::GREEN);
    controller.GetViewfinder().Render(controller, registry, ray_bank);
    Core::Transform view_transform = controller.GetViewfinder().GetViewTransform();
    controller.GetRayBank().Draw(controller.GetGraphics(), view_transform);
    controller.GetRayBank().Clear();
    controller.GetGraphics().PushTransform(view_transform);
    RendererVisitor renderer(controller.GetGraphics());
    controller.GetRegistry().VisitObjects(renderer);
    controller.GetGraphics().PopTransform();
}

void Application::Update(Controller& controller) {
    FocusManager::GetInstance().Update();

    dispatcher->Execute(controller);

    // Dispatch events
    EventStream stream(controller.GetInput());
    while (stream.HasNext()) {
        auto event = stream.Next();
        root.OnEvent(*event);
        editor.OnEvent(*event);
    }

    root.Layout();
    editor.OnUpdate(controller);
    editor.OnRender(controller);
    root.OnRender(controller);

    if (editor.NotFocused() || root.is_hover) {
        DefaultCursor::Update(controller);
    }

    // Do frametime --------------------------------------------
    frametime_counter.Update(controller.GetChronometer().GetDelta());
    float avg_frametime = frametime_counter.GetAverage() * 1000;
    Core::Font& default_font = controller.GetFontManager().GetFont("default");
    Core::FontGraphics font_graphics(controller.GetGraphics(), default_font);

    // format the average frametime to 2 decimal places
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << avg_frametime;
    std::string text = ss.str();
    std::string frametime_string = "Delta: " + text + "ms";

    ss.clear();
    ss.str("");
    ss << std::fixed << std::setprecision(2) << 1.0f / (avg_frametime / 1000.0f);
    text = ss.str();
    std::string fps_string = "FPS: " + text;

    font_graphics.DrawString(Core::Color::GREEN, frametime_string, 0, 0);
    font_graphics.DrawString(Core::Color::GREEN, fps_string, 0, 20);
}

struct SelectButtonHandler : EventHandler {
    Editor& editor;

    SelectButtonHandler(Editor& editor) : editor(editor) {}

    void Handle(MouseClickEvent& event) override { editor.SetInputHandler<SelectionModeHandler>(); }
};

struct TranslateButtonHandler : EventHandler {
    Editor& editor;

    TranslateButtonHandler(Editor& editor) : editor(editor) {}

    void Handle(MouseClickEvent& event) override { editor.SetInputHandler<TranslateModeHandler>(); }
};

struct CopyButtonHandler : EventHandler {
    Editor& editor;

    CopyButtonHandler(Editor& editor) : editor(editor) {}

    void Handle(MouseClickEvent& event) override { editor.SetInputHandler<CopyInputHandler>(); }
};

struct LineModeButtonHandler : EventHandler {
    Editor& editor;

    LineModeButtonHandler(Editor& editor) : editor(editor) {}

    void Handle(MouseClickEvent& event) override { editor.SetInputHandler<LineCreateHandler>(); }
};

struct CircleModeButtonHandler : EventHandler {
    Editor& editor;

    CircleModeButtonHandler(Editor& editor) : editor(editor) {}

    void Handle(MouseClickEvent& event) override { editor.SetInputHandler<CreateCircleHandler>(); }
};

struct GridToggleButtonHandler : EventHandler {
    bool is_grid_on = false;
    Cursor& cursor;

    GridToggleButtonHandler(Cursor& cursor) : cursor(cursor) {}

    void Handle(MouseClickEvent& event) override {
        // toggle the grid
        is_grid_on = !is_grid_on;
        cursor.SetGridSnapped(is_grid_on);
    }
};

Application::Application() {
    registry = std::make_unique<ObjectRegistry>();
    viewfinder = std::make_unique<Viewfinder>();
    dispatcher = std::make_shared<Dispatcher>();
    ray_bank = std::make_unique<RayBank>();

    root.position = Core::Vector2(0, 280);
    root.size = Core::Vector2(200, 275);
    root.direction = Direction::Horizontal;

    auto minimap = std::make_unique<Minimap>();
    minimap->size = Core::Vector2(200, 200);
    // root.Insert(std::move(minimap));

    auto spacer = std::make_unique<Container>();
    spacer->size = Core::Vector2(200, 0);
    // root.Insert(std::move(spacer));

    auto select_button = std::make_unique<Button>();
    select_button->text = "Select";
    select_button->size = Core::Vector2(65, 20);
    select_button->handlers.push_back(std::make_unique<SelectButtonHandler>(editor));
    root.Insert(std::move(select_button));

    auto translate_button = std::make_unique<Button>();
    translate_button->text = "Move";
    translate_button->size = Core::Vector2(65, 20);
    translate_button->handlers.push_back(std::make_unique<TranslateButtonHandler>(editor));
    root.Insert(std::move(translate_button));

    auto copy_button = std::make_unique<Button>();
    copy_button->text = "Copy";
    copy_button->size = Core::Vector2(65, 20);
    copy_button->handlers.push_back(std::make_unique<CopyButtonHandler>(editor));
    root.Insert(std::move(copy_button));

    auto line_button = std::make_unique<Button>();
    line_button->text = "Line";
    line_button->size = Core::Vector2(65, 20);
    line_button->handlers.push_back(std::make_unique<LineModeButtonHandler>(editor));
    root.Insert(std::move(line_button));

    auto circle_button = std::make_unique<Button>();
    circle_button->text = "Circle";
    circle_button->size = Core::Vector2(65, 20);
    circle_button->handlers.push_back(std::make_unique<CircleModeButtonHandler>(editor));
    root.Insert(std::move(circle_button));

    auto grid_button = std::make_unique<Button>();
    grid_button->text = "Grid";
    grid_button->size = Core::Vector2(65, 20);
    grid_button->handlers.push_back(std::make_unique<GridToggleButtonHandler>(viewfinder->GetCursor()));
    root.Insert(std::move(grid_button));
}
} // namespace Cad