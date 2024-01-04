#pragma once

#include <core/graphics/Pixel.h>
#include <cstdint>
#include <vector>

namespace Core {
class Font {
    unsigned m_width, m_height;
    unsigned m_glyph_width;
    std::vector<Pixel> m_pixels;

  public:
    Font(std::vector<Pixel> pixels, unsigned glyph_width);

    unsigned GetGlyphWidth() const;
    bool GetGlyph(unsigned char character, unsigned x, unsigned y) const;

    static Font LoadFromBin(const char* filename, unsigned glyph_width = 16);
};
}