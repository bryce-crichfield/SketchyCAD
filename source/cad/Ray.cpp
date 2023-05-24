
#include <cad/Ray.h>
#include <cad/object/ObjectRegistry.h>
#include <cad/object/ObjectVisitor.h>

namespace Cad {

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
    auto width = graphics.GetWidth();
    auto height = graphics.GetHeight();
    float ray_start_x = ray_origin_screen.x - direction.x * width;
    float ray_start_y = ray_origin_screen.y - direction.y * height;
    float ray_end_x = ray_origin_screen.x + direction.x * width;
    float ray_end_y = ray_origin_screen.y + direction.y * height;
    graphics.DrawDotted(color, ray_start_x, ray_start_y, ray_end_x, ray_end_y, 3);
}

struct RaycastSnapVisitor : public ObjectVisitor {
    std::vector<Core::Vector2> snap_vectors;
    Core::Vector2 mouse_pos;
    Core::Transform view_transform;
    std::vector<Ray> rays;

    RaycastSnapVisitor(std::vector<Ray> rays, Core::Transform transform, Core::Vector2 mouse_pos)
        : rays(rays), view_transform(transform), mouse_pos(mouse_pos)
    {
    }

    void Visit(LineObject& object) override
    {
        // check for line intersection
        for (auto& ray : rays) {
            auto intersection = ray.GetLineIntersection(object.start, object.end);
            if (!intersection.has_value()) return;
            auto intersection_screen = view_transform.Apply(intersection.value());
            float distance_from_mouse = (mouse_pos - intersection_screen).Length();
            if (distance_from_mouse < 10) {
                snap_vectors.push_back(intersection_screen);
            }
        }
    }

    void Visit(CircleObject& object) override
    {
        // check for circle intersection
        // check for circle tangent
    }

    void Visit(PolylineObject& object) override
    {
        // check for line intersection
        // check for line tangent
        // check for circle intersection
        // check for circle tangent
    }

    std::vector<Core::Vector2> CollectResults()
    {
        // return a vector of SnapVectors
        auto result = std::move(snap_vectors);
        snap_vectors.clear();
        return result;
    }
};

std::vector<Core::Vector2> RayBank::GetSnapPoints(
    Cad::ObjectRegistry& registry, Core::Transform view_transform, Core::Vector2 mouse_position)
{
    RaycastSnapVisitor visitor(rays, view_transform, mouse_position);
    registry.VisitObjects(visitor);
    return visitor.CollectResults();
}

}; // namespace Cad