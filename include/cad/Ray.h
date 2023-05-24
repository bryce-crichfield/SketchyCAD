#pragma once

#include <core/Core.h>
#include <optional>

namespace Cad {
struct Ray {
    Core::Vector2 origin;
    Core::Vector2 direction;

    Ray(Core::Vector2 origin, Core::Vector2 direction);
    std::optional<Core::Vector2> GetLineIntersection(Core::Vector2 start, Core::Vector2 end);
    void Draw(Core::Graphics& graphics, Core::Transform view_transform);
};
} // namespace Cad