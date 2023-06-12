#pragma once

#include <cad/gui/Button.h>

namespace Cad {
    
Button::Button() {
    handlers.push_back(std::make_unique<ButtonEventHandler>(*this));
}

} // namespace Cad