#pragma once

#include <cad/gui/Component.h>

namespace Cad {
    
enum class Direction {
    Horizontal,
    Vertical
};

enum class Alignment {
    Start,
    Center,
    End
};

struct Container : public Component {
    std::vector<std::unique_ptr<Component>> children;

    Direction direction = Direction::Vertical;
    Alignment alignment = Alignment::Start;
ww
    void OnEvent(Event& event) override {
        event.Handle(*this);
        if (event.IsHandled()) {
            return;
        }
        for (auto& child : children) {
            child->OnEvent(event);
            if (event.IsHandled()) {
                return;
            }
        }
    }

    void OnRender(Cad::Controller& controller) override {
        for (auto& child : children) {
            child->OnRender(controller);
        }
    }

    void Layout();

    void Insert(std::unique_ptr<Component> component) { children.push_back(std::move(component)); }

    void Handle(FocusEvent& event) override {
        for (auto& child : children) {
            child->Handle(event);
            if (event.IsHandled()) {
                return;
            }
        }
    }
};
} // namespace Cad