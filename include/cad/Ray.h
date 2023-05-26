#pragma once

#include <core/Core.h>
#include <cad/object/ObjectRegistry.h>
#include <optional>

namespace Cad {
struct Ray {
    Core::Vector2 origin;
    Core::Vector2 direction;
    Core::Pixel color;

    Ray(Core::Vector2 origin, Core::Vector2 direction, Core::Pixel color = Core::Color::WHITE)
        : origin(origin), direction(direction), color(color){};

    enum class Intersection {
        None,
        Parallel,
        Collinear,
        Intersecting,
    };

    struct IntersectionResult {
        Intersection intersection;
        Core::Vector2 point;
        IntersectionResult(Intersection intersection)
            : intersection(intersection), point(Core::Vector2(0, 0)){};
        IntersectionResult(Core::Vector2 point)
            : intersection(Intersection::Intersecting), point(point){};
    };

    IntersectionResult GetLineIntersection(Core::Vector2 start, Core::Vector2 end);
    void Draw(Core::Graphics& graphics, Core::Transform view_transform);
    bool operator==(const Ray& other) const
    {
        return origin == other.origin && direction == other.direction;
    }
};

struct RayBank {
    std::vector<Ray> rays;

    RayBank() = default;

    void AddRay(Core::Vector2 origin, Core::Vector2 direction, Core::Pixel color = Core::Color::WHITE)
    {
        rays.emplace_back(origin, direction, color);
    }
    void AddRay(Ray ray) { rays.push_back(ray); }
    void Draw(Core::Graphics& graphics, Core::Transform view_transform)
    {
        for (auto& ray : rays) {
            ray.Draw(graphics, view_transform);
        }
    }

    void Clear() { rays.clear(); }

    std::vector<Core::Vector2> GetSnapPoints(
        Cad::ObjectRegistry& registry, Core::Transform view_transform, Core::Vector2 mouse_position);
};

} // namespace Cad