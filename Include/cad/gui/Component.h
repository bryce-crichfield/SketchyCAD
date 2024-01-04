#pragma once

#include <core/Core.h>
#include <cad/Controller.h>
#include <cad/gui/Event.h>

namespace Cad {
bool Inside(float x, float y, float width, float height, float px, float py);

// Note: I will make this static for now.  The "Controller" notion is for sure already GOD status.
// Therefore, I will avoid bloating more, for now.  This is starting to display systems of dependency
// hell, and that is a problem.  I will need to refactor this soon (hahah).
struct Style {
    Core::Pixel foreground = Core::Color::BLACK;
    Core::Pixel background = Core::Color::WHITE;
    Core::Pixel border = Core::Color::GRAY;

    bool has_border = true;
    bool has_background = true;
    bool has_foreground = true;
};

struct DefaultCursor {
    inline static bool is_visible = true;

    static void Update(Cad::Controller& controller) {
        if (!is_visible) return;

        auto& graphics = controller.GetGraphics();
        auto p0 = controller.GetInput().GetMousePosition();
        auto p1 = p0 + Core::Vector2(0, 5);
        auto p2 = p1 + Core::Vector2(5, -1);

        auto color = Core::Color::LIGHT_GRAY;
        graphics.DrawTriangle(color, p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
    }
};

struct Component : public EventHandler {
    Style style;
    Core::Vector2 position;
    Core::Vector2 size;

    std::vector<std::unique_ptr<EventHandler>> handlers;
    bool is_hover = true;
    bool is_focusable = false;
    bool is_focused = false;

    Component() = default;
    virtual ~Component() {}

    bool IsFocused() const { return is_focused; }

    bool NotFocused() const { return !is_focused; }

    void DispatchToHandlers(Event& event) {
        for (auto& handler : handlers) {
            event.Handle(*handler);
            if (event.IsHandled()) {
                return;
            }
        }
    }

    virtual void OnEvent(Event& event) { event.Handle(*this); }

    virtual void OnRender(Cad::Controller& controller) {}

    void Handle(MouseClickEvent& event) override {
        float x = position.x;
        float y = position.y;
        float width = size.x;
        float height = size.y;
        if (Inside(x, y, width, height, event.x, event.y)) {
            DispatchToHandlers(event);
        }
    }

    void Handle(MouseMoveEvent& event) override {
        float x = position.x;
        float y = position.y;
        float width = size.x;
        float height = size.y;
        if (Inside(x, y, width, height, event.x, event.y)) {
            if (!is_hover) {
                auto enter_event = std::make_unique<MouseEnterEvent>();
                OnEvent(*enter_event);
            }
            DispatchToHandlers(event);
        } else {
            if (is_hover) {
                auto leave_event = std::make_unique<MouseLeaveEvent>();
                OnEvent(*leave_event);
            }
        }
    }

    void Handle(MouseScrollEvent& event) override {
        float x = position.x;
        float y = position.y;
        float width = size.x;
        float height = size.y;
        if (Inside(x, y, width, height, event.dx, event.dy)) {
            DispatchToHandlers(event);
        }
    }

    void Handle(MouseEnterEvent& event) override {
        is_hover = true;
        DispatchToHandlers(event);
    }

    void Handle(MouseLeaveEvent& event) override {
        is_hover = false;
        DispatchToHandlers(event);
    }

    // For text and key related events, the component will not receive them,
    // if the component is not focused.
    void Handle(KeyPressEvent& event) override {
        if (NotFocused()) return;
        DispatchToHandlers(event);
    }
    void Handle(KeyReleaseEvent& event) override {
        if (NotFocused()) return;
        DispatchToHandlers(event);
    }
    void Handle(KeyRepeatEvent& event) override {
        if (NotFocused()) return;
        DispatchToHandlers(event);
    }
    void Handle(TextInputEvent& event) override {
        if (NotFocused()) return;
        DispatchToHandlers(event);
    }
    void Handle(FocusEvent& event) override {
        if (!is_focusable) return;

        if (event.type == FocusEvent::Type::Gained) {
            is_focused = true;
            event.SetHandled();
        } else {
            is_focused = false;
        }

        DispatchToHandlers(event);
    }
};
}