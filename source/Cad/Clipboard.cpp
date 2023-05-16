#include <Cad.h>

void Cad::Clipboard::Foreach(Controller& controller, std::function<void(unsigned, IObject&)> callback)
{
    for (unsigned id : objects) {
        auto opt_obj = controller.GetWorld().Get(id);
        if (opt_obj.has_value()) {
            callback(id, *opt_obj.value());
        }
    }
}