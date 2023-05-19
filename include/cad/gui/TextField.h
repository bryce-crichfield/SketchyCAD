#pragma once

#include <cad/Cad.h>
#include <core/Core.h>

namespace Cad::Gui {
struct TextField : public Core::Gui::Panel {
    std::shared_ptr<Cad::Dispatcher> command_dispatcher;
    Core::Typist typist;
    std::string leader = "> ";

    TextField(std::shared_ptr<Cad::Dispatcher> dispatcher, unsigned width, unsigned height)
        : command_dispatcher(dispatcher), Panel(width, height)
    {
    }

    void OnInput(Core::Controller& controller) override
    {
        auto& output = controller.GetOutput();
        typist.Update(controller);

        if (controller.GetInput().IsPressed(Core::Key::Enter)) {
            output.Writeln(leader + typist.GetText());
            command_dispatcher->Dispatch(typist.GetText(), output);
            // assuming not nullptr because of single constructor
            typist.Clear();
        }

        if (controller.GetInput().IsHeld(Core::Key::LControl) &&
            controller.GetInput().IsPressed(Core::Key::Backspace)) {
            typist.Clear();
        }
    }

    void OnUpdate(Core::Controller& state) override {}

    void OnRender(Core::Controller& controller) override
    {
        float x = GetPosition().x;
        float y = GetPosition().y;
        float width = GetSize().x;
        float height = GetSize().y;

        Core::Font& font = controller.GetFontManager().GetFont("default");
        font.GetGlyphWidth();
        Core::FontGraphics font_graphics(controller.GetGraphics(), font);

        // Draw background and border
        controller.GetGraphics().FillRect(Core::Color::BLACK, x, y, width, height);
        controller.GetGraphics().DrawLine(Core::Color::WHITE, x, y, x, y + height);
        controller.GetGraphics().DrawLine(Core::Color::WHITE, x + width, y, x + width, y + height);
        controller.GetGraphics().DrawLine(Core::Color::WHITE, x, y + height, x + width, y + height);

        // Calculate the max text length, drop the first n characters if it is too long
        unsigned font_size = font_graphics.GetDisplayWidth();
        unsigned max_text_length = (width - font_size) / font_size;
        std::string text = leader + typist.GetText();
        if (text.length() > max_text_length) {
            text = text.substr(text.length() - max_text_length, max_text_length);
        }
        // // Re-add the leader character
        text = leader + text.substr(leader.length(), text.length() - leader.length());
        float text_y = y + (height - font_size) / 2;
        // // Draw the text at the center of the text field
        font_graphics.DrawString(Core::Color::GREEN, text, x, text_y);
    }
};
} // namespace Cad::Gui