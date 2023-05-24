
#include <cad/Ray.h>

namespace Cad {
    Ray::Ray(Core::Vector2 origin, Core::Vector2 direction) : origin(origin), direction(direction) {}

    std::optional<Core::Vector2> Ray::GetLineIntersection(Core::Vector2 start, Core::Vector2 end)
    {
        Core::Vector2 p = origin;
        p = p - (direction * Core::Vector2(1000, 1000));
        Core::Vector2 r = (direction * Core::Vector2(1000, 1000)) * 2;

        Core::Vector2 q = start;
        Core::Vector2 s = end - start;

        float r_cross_s = r.Cross(s);
        float q_minus_p_cross_r = (q - p).Cross(r);

        if (r_cross_s == 0 && q_minus_p_cross_r == 0) {
            // lines are collinear
            return std::nullopt;
        }

        if (r_cross_s == 0 && q_minus_p_cross_r != 0) {
            // lines are parallel
            return std::nullopt;
        }

        float t = (q - p).Cross(s) / r_cross_s;
        float u = (q - p).Cross(r) / r_cross_s;

        if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
            auto result = p + (r * t);
            return result;
        }

        if (t >= 0 && t <= 1) {
            // lines intersect, but segments do not
            return std::nullopt;
        }

        // lines do not intersect
        return std::nullopt;
    }

    void Ray::Draw(Core::Graphics& graphics, Core::Transform view_transform)
    {
        Core::Vector2 ray_origin_screen = view_transform.Apply(origin);
        // TODO: Remove magic numbers
        float ray_start_x = ray_origin_screen.x - direction.x * 1000;
        float ray_start_y = ray_origin_screen.y - direction.y * 1000;
        float ray_end_x = ray_origin_screen.x + direction.x * 1000;
        float ray_end_y = ray_origin_screen.y + direction.y * 1000;
        graphics.DrawDotted(Core::Color::PURPLE, ray_start_x, ray_start_y, ray_end_x, ray_end_y, 3);
    }
};