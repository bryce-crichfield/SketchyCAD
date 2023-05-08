#include <Engine.h>
#include <fstream>

Font::Font(std::vector<Pixel> pixels, unsigned glyph_width) : m_pixels(pixels), m_glyph_width(glyph_width)
{
    m_width = m_glyph_width * 16;
    m_height = m_glyph_width * 16;
}

unsigned Font::GetGlyphWidth() const
{
    return m_glyph_width;
}

bool Font::GetGlyph(unsigned char character, unsigned x, unsigned y) const
{
    if (x >= m_width || y >= m_height)
        return false;

    unsigned glyph_x = character % 16;
    unsigned glyph_y = character / 16;

    unsigned glyph_offset_x = glyph_x * m_glyph_width;
    unsigned glyph_offset_y = glyph_y * m_glyph_width;

    unsigned pixel_x = glyph_offset_x + x;
    unsigned pixel_y = glyph_offset_y + y;

    unsigned pixel_index = pixel_y * m_width + pixel_x;

    bool is_white = m_pixels[pixel_index].r > 0;

    return is_white;
}

Font Font::LoadFromBin(const char *filename, unsigned glyph_width)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open file");
    }

    std::vector<uint8_t> buffer(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read((char *)buffer.data(), buffer.size());
    file.close();

    std::vector<Pixel> pixels;
    for (unsigned i = 0; i < buffer.size(); i += 3)
    {
        pixels.push_back(Pixel(buffer[i], buffer[i + 1], buffer[i + 2]));
    }
    
    return Font(pixels, glyph_width);
}

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
                graphics.DrawPixel(color, x + xi, y + yi);
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