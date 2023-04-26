#include <Engine.h>

Image::Image(unsigned width, unsigned height)
{
    m_width = width;
    m_height = height;
    m_data = new Pixel[width * height];
}

Image::~Image()
{
    delete[] m_data;
}

uint8_t *Image::GetData()
{
    return (uint8_t *)m_data;
}

unsigned Image::GetWidth() const
{
    return m_width;
}

unsigned Image::GetHeight() const
{
    return m_height;
}

Pixel Image::GetPixel(unsigned x, unsigned y) const
{
    if (x >= m_width || y >= m_height)
    {
        return Color::BLACK;
    }

    return m_data[y * m_width + x];
}

void Image::SetPixel(unsigned x, unsigned y, Pixel pixel)
{
    if (x >= m_width || y >= m_height)
    {
        return;
    }

    m_data[y * m_width + x] = pixel;
}

Pixel Image::SamplePixel(float x, float y) const {
    x = std::clamp(x, 0.0f, 1.0f);
    y = std::clamp(y, 0.0f, 1.0f);
    unsigned sample_x = floor(x * m_width);
    unsigned sample_y = floor(y * m_height);
    return m_data[sample_y * m_width + sample_x];
}

void Image::Clear(const Pixel &pixel)
{
    for (unsigned i = 0; i < m_width * m_height; i++)
    {
        m_data[i] = pixel;
    }
}

Graphics &Image::GetGraphics()
{
    std::shared_ptr<Image> image = std::make_shared<Image>(*this);
    return *(new Graphics(image));
}