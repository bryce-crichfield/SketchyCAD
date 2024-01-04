#pragma once

#include <core/font/Font.h>
#include <core/graphics/Graphics.h>

#include <cstdint>
#include <string>

namespace Core {
class FontGraphics {
    unsigned m_display_width = 8;
    Font& font;
    Graphics& graphics;

  public:
    FontGraphics(Graphics& graphics, Font& font);

    unsigned GetDisplayWidth() const { return m_display_width; }
    void SetDisplayWidth(unsigned width) { m_display_width = width; }
    void DrawGlyph(Pixel color, unsigned char character, unsigned x, unsigned y);
    void DrawString(Pixel color, std::string str, unsigned x, unsigned y);
    Vector2 MeasureString(std::string str) const;
};
}