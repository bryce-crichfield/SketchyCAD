
#include <cad/Ray.h>
#include <cad/object/ObjectRegistry.h>
#include <cad/object/ObjectVisitor.h>

#define MIN_INT -2147483648
#define MAX_INT 2147483647

namespace Cad {

Ray::IntersectionResult Ray::GetLineIntersection(Core::Vector2 start, Core::Vector2 end) {
    Core::Vector2 p = origin;
    p = p - (direction * Core::Vector2(1000, 1000));
    Core::Vector2 r = (direction * Core::Vector2(1000, 1000)) * 2;

    Core::Vector2 q = start;
    Core::Vector2 s = end - start;

    float r_cross_s = r.Cross(s);
    float q_minus_p_cross_r = (q - p).Cross(r);

    if (r_cross_s == 0 && q_minus_p_cross_r == 0) {
        // lines are collinear
        return Ray::IntersectionResult(Ray::Intersection::Collinear);
    }

    if (r_cross_s == 0 && q_minus_p_cross_r != 0) {
        // lines are parallel
        return Ray::IntersectionResult(Ray::Intersection::Parallel);
    }

    float t = (q - p).Cross(s) / r_cross_s;
    float u = (q - p).Cross(r) / r_cross_s;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        auto result = p + (r * t);
        return Ray::IntersectionResult(result);
    }

    if (t >= 0 && t <= 1) {
        // lines intersect, but segments do not
        return Ray::IntersectionResult(Ray::Intersection::None);
    }

    // lines do not intersect
    return Ray::IntersectionResult(Ray::Intersection::None);
}

void Ray::Draw(Core::Graphics& graphics, Core::Transform view_transform) {
    // the start and end points are where the ray intersects the view
    auto origin_screen = view_transform.Apply(origin);
    float width = graphics.GetWidth();
    width *= view_transform.scale;
    float height = graphics.GetHeight();
    height *= view_transform.scale;

    float start_x = origin_screen.x - (direction.x * width);
    float start_y = origin_screen.y - (direction.y * height);
    float end_x = origin_screen.x + (direction.x * width);
    float end_y = origin_screen.y + (direction.y * height);

    // TODO: this works better but at low zoom it breaks down
    graphics.DrawLine(color,start_x, start_y, end_x, end_y);
}

struct RaycastSnapVisitor : public ObjectVisitor {
    std::vector<Core::Vector2> snap_vectors;
    Core::Vector2 mouse_pos;
    Core::Transform view_transform;
    std::vector<Ray> rays;

    RaycastSnapVisitor(std::vector<Ray> rays, Core::Transform transform, Core::Vector2 mouse_pos)
        : rays(rays), view_transform(transform), mouse_pos(mouse_pos) {}

    void Visit(LineObject& object) override {
        // check for line intersection
        for (auto& ray : rays) {
            auto intersection = ray.GetLineIntersection(object.start, object.end);
            if (intersection.intersection != Ray::Intersection::Intersecting) return;
            auto intersection_screen = view_transform.Apply(intersection.point);
            float distance_from_mouse = (mouse_pos - intersection_screen).Length();
            if (distance_from_mouse < 10) {
                snap_vectors.push_back(intersection_screen);
            }
        }
    }

    void Visit(CircleObject& object) override {
        // check for circle intersection
        // check for circle tangent
    }

    void Visit(PolylineObject& object) override {
        // check for line intersection
        // check for line tangent
        // check for circle intersection
        // check for circle tangent
    }

    std::vector<Core::Vector2> CollectResults() {
        // return a vector of SnapVectors
        auto result = std::move(snap_vectors);
        snap_vectors.clear();
        return result;
    }
};

std::vector<Core::Vector2> RayBank::GetSnapPoints(
    Cad::ObjectRegistry& registry, Core::Transform view_transform, Core::Vector2 mouse_position) {
    RaycastSnapVisitor visitor(rays, view_transform, mouse_position);
    registry.VisitObjects(visitor);
    auto results = visitor.CollectResults();
    // check for any ray to ray intersections

    for (auto& ray : rays) {
        for (auto& other_ray : rays) {
            // if (ray == other_ray) continue;
            auto intersection = ray.GetLineIntersection(other_ray.origin, other_ray.origin + other_ray.direction);
            if (intersection.intersection != Ray::Intersection::Intersecting) continue;

            auto intersection_screen = view_transform.Apply(intersection.point);
            float distance_from_mouse = (mouse_position - intersection_screen).Length();
            if (distance_from_mouse < 10) {
                results.push_back(intersection_screen);
            }
        }
    }

    return results;
}

}; // namespace Cad