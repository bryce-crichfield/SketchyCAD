#pragma once

#include <core/Core.h>
#include <cad/Cad.h>

namespace Cad::Gui {
struct TextDisplay : public Core::Gui::Panel {
    float padding = 3;

    TextDisplay(unsigned width, unsigned height) : Core::Gui::Panel(width, height) {}

    void OnInput(Core::Controller& controller) override {}

    void OnUpdate(Core::Controller& controller) override {}

    void OnRender(Core::Controller& controller) override
    {
        float x = GetPosition().x;
        float y = GetPosition().y;
        float w = GetSize().x;
        float h = GetSize().y;

        Core::Font& font = controller.GetFontManager().GetFont("default");
        Core::FontGraphics font_graphics(controller.GetGraphics(), font);
        Core::Graphics& graphics = controller.GetGraphics();
        graphics.FillRect(Core::Color::BLACK, x, y, w, h);
        graphics.DrawRect(Core::Color::WHITE, x, y, w, h);

        std::vector<std::string> lines;
        std::string output = controller.GetOutput().Dump();
        std::string line = "";
        for (auto character : output) {
            if (character == '\n') {
                lines.push_back(line);
                line = "";
            } else {
                line += character;
            }
        }
        lines.push_back(line);
        
        unsigned font_size = font_graphics.GetDisplayWidth();
        unsigned max_lines = (h - (2 * padding)) / font_size;
        unsigned max_chars = (w - (2 * padding)) / font_size;

        unsigned start_x = x + padding;
        unsigned start_y = (y + h) - (padding + font_size);
        unsigned end_x = (x + w) - (padding + font_size);
        unsigned end_y = y + (padding + font_size);

        unsigned draw_x = start_x;
        unsigned draw_y = start_y;

        unsigned delta_x = font_size;
        unsigned delta_y = -1 * font_size;

        // Draw the lines in reverse order
        for (auto it = lines.rbegin(); it != lines.rend(); it++) {
            for (auto character : (*it)) {
                std::string character_string = std::string(1, character);
                font_graphics.DrawString(Core::Color::WHITE, character_string, draw_x, draw_y);
                bool is_overline = draw_x > end_x;
                if (is_overline) {
                    draw_x = start_x;
                    draw_y += delta_y;
                } else {
                    draw_x += delta_x;
                }

                if (draw_y < end_y) {
                    return;
                }
            }

            draw_x = start_x;
            draw_y += delta_y;
            draw_y -= 5;
        }
    }
};
} // namespace Cad::Gui