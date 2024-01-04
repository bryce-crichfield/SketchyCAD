#include <core/font/FontGraphics.h>

namespace Core {

FontGraphics::FontGraphics(Graphics &graphics, Font &font) : graphics(graphics), font(font){};

void FontGraphics::DrawGlyph(Pixel color, unsigned char character, unsigned x, unsigned y)
{
    unsigned width = m_display_width;
    for (unsigned yi = 0; yi < width; yi++)
    {
        for (unsigned xi = 0; xi < width; xi++)
        {
            float u = (float)xi / (float)width;
            float v = (float)yi / (float)width;
            unsigned pixel_x = (unsigned)floor(u * font.GetGlyphWidth());
            unsigned pixel_y = (unsigned)floor(v * font.GetGlyphWidth());
            unsigned pixel_index = pixel_y * font.GetGlyphWidth() + pixel_x;
            auto glyph_pixel = font.GetGlyph(character, pixel_x, pixel_y);
            if (glyph_pixel)
            {
                graphics.SetPixel(color, x + xi, y + yi);
            }
        }
    }
}

void FontGraphics::DrawString(Pixel color, std::string str, unsigned x, unsigned y)
{
    unsigned width = m_display_width;
    for (unsigned i = 0; i < str.size(); i++)
    {
        unsigned write_x = x + i * width;
        DrawGlyph(color, str[i], write_x, y);
    }
}

Vector2 FontGraphics::MeasureString(std::string str) const
{
    unsigned width = m_display_width;
    return Vector2(str.size() * width, width);
}
}