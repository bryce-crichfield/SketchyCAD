#include <Engine.h>

Pixel::Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

Pixel Pixel::Darker() const
{
    return Pixel(r / 2, g / 2, b / 2, a);
}

Pixel Pixel::Lighter() const
{
    return Pixel(r + (255 - r) / 2, g + (255 - g) / 2, b + (255 - b) / 2, a);
}