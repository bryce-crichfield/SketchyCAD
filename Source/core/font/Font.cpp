#include <core/font/Font.h>
#include <fstream>
#include <iostream>


namespace Core {

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
}