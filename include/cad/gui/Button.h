#pragma once

#include <cad/gui/Component.h>

namespace Cad {
struct wButton : public Component {
    std::string text = "Button";
    Button();
    virtual ~Button() {}
    void OnRender(Cad::Controller& controller) override {
        float x = position.x;
        float y = position.y;
        float width = size.x;
        float height = size.y;
        auto& graphics = controller.GetGraphics();
        graphics.FillRect(style.background, x, y, width, height);
        graphics.DrawRect(style.border, x, y, width, height);

        Core::Font& font = controller.GetFontManager().GetFont("default");
        Core::FontGraphics font_graphics(controller.GetGraphics(), font);
        float center_x = x + width / 2.0f;
        float center_y = y + height / 2.0f;
        float font_height = font_graphics.GetDisplayWidth();
        float string_width = font_graphics.MeasureString(text).x;
        font_graphics.DrawString(style.foreground, text, center_x - string_width / 2.0f, center_y - font_height / 2.0f);
    }

    virtual void OnClick() {}
};

struct ButtonEventHandler : EventHandler {
    Button& button;
    ButtonEventHandler(Button& button) : button(button) {}
    void Handle(MouseClickEvent& event) override { button.OnClick(); }
};
} // namespace Cad