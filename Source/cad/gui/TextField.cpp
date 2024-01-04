#include <cad/gui/TextField.h>

namespace Cad {

TextField::TextField() {
    is_focusable = true;
    handlers.push_back(std::make_unique<TextFieldEventHandler>(*this));
}

} // namespace Cad
