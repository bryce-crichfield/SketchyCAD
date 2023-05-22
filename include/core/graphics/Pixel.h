#pragma once

#include <cstdint>

namespace Core {
struct Pixel {
    uint8_t r, g, b, a = 0;

    Pixel() = default;
    Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    Pixel Darker() const;
    Pixel Lighter() const;
};

namespace Color {
const Pixel BLACK(0x21, 0x21, 0x21);
const Pixel DARK_GRAY(64, 64, 64);
const Pixel GRAY(128, 128, 128);
const Pixel LIGHT_GRAY(192, 192, 192);
const Pixel WHITE(255, 255, 220);
const Pixel RED(136, 0, 0);
const Pixel CYAN(170, 255, 238);
const Pixel PURPLE(204, 68, 204);
const Pixel GREEN(0, 204, 85);
const Pixel BLUE(0, 0, 170);
const Pixel YELLOW(238, 238, 119);
const Pixel ORANGE(221, 136, 85);
const Pixel BROWN(102, 68, 0);
} // namespace Color
} // namespace Core