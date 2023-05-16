#include <Cad.h>

namespace Cad::Command {

void Create::Forward(Controller& controller)
{
    std::optional<unsigned>  is_registered = BuildAndRegister(controller);
    if (!is_registered.has_value()) return;
    obj_id = is_registered.value();
    is_forward = true;
    controller.GetStaticContext().output.Println("[INFO]: Created object " + std::to_string(obj_id));
}

void Create::Backward(Controller& controller)
{
    if (!is_forward) {
        return;
    }

    is_forward = false;
    controller.GetWorld().Remove(obj_id);
}

std::optional<unsigned> CreateLine::BuildAndRegister(Controller& controller) {
    auto opt_id = controller.GetWorld().GetNextId();
    if (!opt_id.has_value()) {
        return std::nullopt;
    }
    auto obj = std::make_unique<Cad::Object::Line>(opt_id.value(), start, end);
    controller.GetWorld().Register(opt_id.value(), std::move(obj));
    return opt_id.value();
}

std::optional<unsigned> CreateCircle::BuildAndRegister(Controller& controller)
{
    float r = (center - radius).Length();
    auto opt_id = controller.GetWorld().GetNextId();
    if (!opt_id.has_value()) {
        return std::nullopt;
    }
    auto obj = std::make_unique<Cad::Object::Circle>(opt_id.value(), center, r);
    controller.GetWorld().Register(opt_id.value(), std::move(obj));
    return opt_id.value();
}

} // namespace Cad::Command