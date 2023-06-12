#include <core/graphics/Image.h>
#include <core/graphics/ImageGraphics.h>

#include <algorithm>

namespace Core {

Image::Image(unsigned width, unsigned height) {
    m_width = width;
    m_height = height;
    m_data = new Pixel[width * height];
}

Image::~Image() { delete[] m_data; }

uint8_t* Image::GetData() { return (uint8_t*)m_data; }

unsigned Image::GetWidth() const { return m_width; }

unsigned Image::GetHeight() const { return m_height; }

Pixel Image::GetPixel(unsigned x, unsigned y) const {
    if (x >= m_width || y >= m_height) {
        return Color::BLACK;
    }

    return m_data[y * m_width + x];
}

void Image::SetPixel(unsigned x, unsigned y, Pixel pixel) {
    if (x >= m_width || y >= m_height) {
        return;
    }

    m_data[y * m_width + x] = pixel;
}

Pixel Image::SamplePixel(float x, float y, SampleMode mode) const {
    if (mode == Image::SampleMode::NEAREST) {
        x = std::clamp(x, 0.0f, 1.0f);
        y = std::clamp(y, 0.0f, 1.0f);
        unsigned sample_x = floor(x * m_width);
        unsigned sample_y = floor(y * m_height);
        return m_data[sample_y * m_width + sample_x];
    }
    
    if (mode == Image::SampleMode::BILINEAR) {
        float sample_x = x * m_width;
        float sample_y = y * m_height;
        unsigned x0 = floor(sample_x);
        unsigned y0 = floor(sample_y);
        unsigned x1 = ceil(sample_x);
        unsigned y1 = ceil(sample_y);
        float x_diff = sample_x - x0;
        float y_diff = sample_y - y0;
        Pixel p00 = GetPixel(x0, y0);
        Pixel p01 = GetPixel(x0, y1);
        Pixel p10 = GetPixel(x1, y0);
        Pixel p11 = GetPixel(x1, y1);

        Pixel p0 = p00 * (1 - x_diff) + p10 * x_diff;
        Pixel p1 = p01 * (1 - x_diff) + p11 * x_diff;

        return p0 * (1 - y_diff) + p1 * y_diff;
    }
}

void Image::Clear(const Pixel& pixel) {
    for (unsigned i = 0; i < m_width * m_height; i++) {
        m_data[i] = pixel;
    }
}

} // namespace Core