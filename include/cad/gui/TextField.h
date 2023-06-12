#pragma once

#include <cad/gui/Component.h>
#include <cad/gui/Focus.h>

namespace Cad {

struct TextField : public Component {
    Core::Typist typist;
    TextField();
    void OnRender(Cad::Controller& controller) override {
        auto& graphics = controller.GetGraphics();

        {
            float x = position.x;
            float y = position.y;
            float width = size.x;
            float height = size.y;
            auto& graphics = controller.GetGraphics();
            graphics.FillRect(style.background, x, y, width, height);
            graphics.DrawRect(style.border, x, y, width, height);
        }

        auto& font = controller.GetFontManager().GetFont("default");
        Core::FontGraphics font_graphics(graphics, font);
        float char_width = font_graphics.GetDisplayWidth();
        float width = size.x;
        float str_width = char_width * typist.GetText().size();
        str_width = std::min(str_width, width);

        float x = position.x;
        float y = position.y + size.y / 2 - char_width / 2;
        float last_n_chars = str_width / char_width;
        float start = typist.GetText().size() - last_n_chars;
        start = std::max(start, 0.0f);
        std::string display_text = typist.GetText().substr(start, typist.GetText().size());
        font_graphics.DrawString(style.foreground, display_text, x, y);
    }

    Core::Typist& GetTypist() { return typist; }
};

struct TextFieldEventHandler : EventHandler {
    TextField& text_field;

    TextFieldEventHandler(TextField& text_field) : text_field(text_field) {}

    void Handle(MouseClickEvent& event) override {
        auto& focus = FocusManager::GetInstance();
        focus.RequestFocus(text_field);
    }

    void Handle(TextInputEvent& event) override { 
       // TODO: In order to adapt the Core classes, we need a way to provide a controller from within this context.
       // I think it is time we consider making the core stuff static.  
    }
};
}