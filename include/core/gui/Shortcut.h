#pragma once

#include <core/input/Key.h>
#include <core/input/Input.h>
#include <functional>


namespace Core::Gui {
using Shortcut = std::function<bool(Input&)>;

namespace Shortcuts {
const Shortcut ctrl_n1 = [](Input& input) { return input.IsHeld(Key::LControl) && input.IsPressed(Key::Num1); };
const Shortcut ctrl_n2 = [](Input& input) { return input.IsHeld(Key::LControl) && input.IsPressed(Key::Num2); };
} // namespace Shortcuts
}
