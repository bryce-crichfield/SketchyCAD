#pragma once

#include <core/graphics/Pixel.h>
#include <core/graphics/Graphics.h>

#include <memory>

namespace Core {
class Image {
    Pixel* m_data;
    unsigned m_width, m_height;

  public:
    enum class SampleMode {
        NEAREST,
        BILINEAR,
        CUBIC,
    };

    Image(unsigned width, unsigned height);
    ~Image();

    uint8_t* GetData();
    unsigned GetWidth() const;
    unsigned GetHeight() const;

    Pixel GetPixel(unsigned x, unsigned y) const;
    Pixel SamplePixel(float x, float y, SampleMode mode = SampleMode::NEAREST) const;
    void SetPixel(unsigned x, unsigned y, Pixel pixel);
    void Clear(const Pixel& pixel = Color::BLACK);
};
}