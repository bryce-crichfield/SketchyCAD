#pragma once

#include <core/gui/Component.h>
#include <core/gui/LayoutStrategy.h>
#include <core/Controller.h>

namespace Core::Gui {
class LayoutStrategy;

class Panel : public Component {
    std::vector<std::shared_ptr<Panel>> m_children;
    std::unique_ptr<LayoutStrategy> m_layout_strategy;

  public:
    Panel(unsigned width, unsigned height);

    void Input(Controller& controller);
    void Update(Controller& controller);
    void Render(Controller& controller);

    virtual void OnInput(Controller& controller) {}
    virtual void OnRender(Controller& controller) {}
    virtual void OnUpdate(Controller& controller) {}

    void AddChild(std::shared_ptr<Panel> child);
    void SetLayoutStrategy(std::unique_ptr<LayoutStrategy> layout_strategy)
    {
        m_layout_strategy = std::move(layout_strategy);
    }
};
}