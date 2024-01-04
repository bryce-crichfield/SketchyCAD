#pragma once

#include <core/input/Input.h>
#include <core/output/Output.h>
#include <core/chrono/Chronometer.h>
#include <core/graphics/Graphics.h>
#include <core/font/FontManager.h>

namespace Core {

struct Controller {
    virtual Input& GetInput() = 0;
    virtual Output& GetOutput() = 0;
    virtual Chronometer& GetChronometer() = 0;
    virtual Graphics& GetGraphics() = 0;
    virtual FontManager& GetFontManager() = 0;
};

} // namespace Core