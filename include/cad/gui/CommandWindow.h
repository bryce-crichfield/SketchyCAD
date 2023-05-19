#pragma once

#include <cad/Cad.h>
#include <core/Core.h>
#include <cad/gui/TextField.h>
#include <cad/gui/TextDisplay.h>


namespace Cad::Gui {

struct CommandWindow : public Core::Gui::Window {
    std::shared_ptr<TextField> text_field;
    std::shared_ptr<TextDisplay> text_display;
    std::shared_ptr<Cad::Dispatcher> command_dispatcher;

    CommandWindow(Core::Gui::InteractionLock& lock, std::shared_ptr<Cad::Dispatcher> dispatcher)
        : Window(lock, Core::Gui::Shortcuts::ctrl_n1, 30, 30, 100, 100), command_dispatcher(dispatcher)
    {
        text_field = std::make_shared<TextField>(dispatcher, 100, 15);
        text_display = std::make_shared<TextDisplay>(100, 80);
        GetRoot().AddChild(text_display);
        GetRoot().AddChild(text_field);
    }

    void OnInput(Core::Controller& controller) override {}

    void OnUpdate(Core::Controller& controller) override
    {
        text_field->SetSize(GetSize().x, 15);
        text_display->SetSize(GetSize().x, GetSize().y - 15);
    }

    void OnRender(Core::Controller& controller) override {}
};
}; // namespace Cad::Gui