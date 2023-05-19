#pragma once

#include <core/gui/Component.h>
#include <core/gui/InteractionLock.h>
#include <core/gui/Shortcut.h>
#include <core/gui/Panel.h>

namespace Core::Gui {
class Window : public Component {
    class DragBar : public Component {
        bool is_being_dragged = false;
        bool has_lock = false;

      public:
        DragBar(unsigned width, unsigned height) : Component(width, height) {}

        void Update(Controller& controller, Window& parent, std::string title);
    };

    std::string m_title = "";
    DragBar m_drag_bar;
    std::unique_ptr<Panel> m_root_panel;

    Shortcut m_toggle_shortcut;
    bool m_is_open = false;

    Interactor static_interactor;

  public:
    Window(InteractionLock& lock, Shortcut toggle_shortcut, unsigned x, unsigned y, unsigned w, unsigned h)
        : m_toggle_shortcut(toggle_shortcut), m_drag_bar(w, 10), static_interactor(lock), Component(w, h)
    {
        m_root_panel = std::make_unique<Panel>(w, h);
    }

    void Update(Controller& controller);
    virtual void OnInput(Controller& controller) = 0;
    virtual void OnUpdate(Controller& controller) = 0;
    virtual void OnRender(Controller& controller) = 0;

    Panel& GetRoot() const { return *m_root_panel; }

    void SetTitle(std::string title) { this->m_title = title; }
};
}