#pragma once

#include <core/Core.h>

#include <cad/Controller.h>
#include <cad/Dispatcher.h>
#include <cad/Viewfinder.h>
#include <cad/gui/Gui.h>
#include <cad/object/ObjectRegistry.h>

#include <iomanip>

namespace Cad {

class InputHandler {
  public:
    virtual ~InputHandler() = default;
    virtual void OnInput(Cad::Controller& controller) = 0;
};

class NoOpInputHandler : public InputHandler {
  public:
    void OnInput(Cad::Controller& controller) override {}
};

struct FrametimeCounter {
    // Averages out the last 60 seconds of frames
    Core::Duration total = 0;
    Core::Duration time_since_reset = 0;
    float average = 0;
    float count = 0;

    bool Update(Core::Duration delta) {
        total = total + delta;
        time_since_reset = time_since_reset + delta;
        count++;
        auto time_since_sec = time_since_reset.AsSeconds();
        if (time_since_sec > 1) {
            time_since_reset = 0;
            average = total.AsSeconds() / count;
            return true;
        }
        return false;
    }

    float GetAverage() { return average; }
};

struct Editor : public Component {
    std::unique_ptr<InputHandler> input_handler;
    bool idle = false;

    Editor();

    void OnStart(Cad::Controller& controller) {
        position = Core::Vector2(0, 0);
        float width = controller.GetGraphics().GetWidth();
        float height = controller.GetGraphics().GetHeight();
        size = Core::Vector2(width, height);
    }

    void OnUpdate(Cad::Controller& controller);
    void OnRender(Cad::Controller& controller);

    template <typename T> void SetInputHandler() {
        input_handler = std::make_unique<T>();
        idle = true;
    }
};

struct EditorHandler : public EventHandler {
    Editor& editor;
    EditorHandler(Editor& editor) : editor(editor) {}
    void Handle(MouseClickEvent& event) override {
        auto& focus = FocusManager::GetInstance();
        focus.RequestFocus(editor);
    }

    void Handle(FocusEvent& event) override {
        bool is_gain = event.type == FocusEvent::Type::Gained;
        if (!is_gain) {
            editor.input_handler = std::make_unique<NoOpInputHandler>();
        }
    }
};

class Application {
    std::unique_ptr<ObjectRegistry> registry;
    std::unique_ptr<Viewfinder> viewfinder;
    std::shared_ptr<Dispatcher> dispatcher;
    std::unique_ptr<RayBank> ray_bank;
    FrametimeCounter frametime_counter;
    Container root;
    Container root2;
    Editor editor;

  public:
    Application();

    void OnStart(Controller& controller);
    void Update(Controller& controller);

    Cad::Controller CreateController(Core::Controller& controller) {
        return Cad::Controller(controller, *registry, *viewfinder, *ray_bank);
    }

    std::shared_ptr<Dispatcher>& GetDispatcher() { return dispatcher; }
};

} // namespace Cad