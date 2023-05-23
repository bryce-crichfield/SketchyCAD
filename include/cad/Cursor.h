#pragma once

#include <cad/object/Object.h>
#include <cad/object/ObjectRegistry.h>
#include <cad/object/ObjectVisitor.h>
#include <core/Core.h>
#include <optional>

namespace Cad {

struct Reticle {
    virtual ~Reticle() = default;
    virtual void Draw(Core::Controller& controller, Core::Vector2 screen_point)
    {
        auto& graphics = controller.GetGraphics();
        float x = screen_point.x;
        float y = screen_point.y;
        graphics.DrawLine(Core::Color::WHITE, x - 10, y, x - 3, y);
        graphics.DrawLine(Core::Color::WHITE, x + 3, y, x + 10, y);
        graphics.DrawLine(Core::Color::WHITE, x, y - 10, x, y - 3);
        graphics.DrawLine(Core::Color::WHITE, x, y + 3, x, y + 10);
    }
};

enum class ReticleType {
    Grid,
    Midpoint,
    Endpoint,
    Intersection,
    None
};

struct SnapVector {
    Core::Vector2 screen_point;
    ReticleType type;

    SnapVector(Core::Vector2 screen_point, ReticleType type) : screen_point(screen_point), type(type) {}
};

struct GridpointReticle : public Reticle {
    GridpointReticle() = default;
    void Draw(Core::Controller& controller, Core::Vector2 screen_point) override
    {
        Reticle::Draw(controller, screen_point);
        auto& graphics = controller.GetGraphics();
        auto screen_cursor = screen_point;
        float topleft_x = screen_cursor.x - 2;
        float topleft_y = screen_cursor.y - 2;
        float bottomright_x = screen_cursor.x + 2;
        float bottomright_y = screen_cursor.y + 2;

        graphics.DrawLine(Core::Color::WHITE, topleft_x, topleft_y, topleft_x + 1, topleft_y);
        graphics.DrawLine(Core::Color::WHITE, topleft_x, topleft_y, topleft_x, topleft_y + 1);
        graphics.DrawLine(Core::Color::WHITE, bottomright_x, bottomright_y, bottomright_x - 1, bottomright_y);
        graphics.DrawLine(Core::Color::WHITE, bottomright_x, bottomright_y, bottomright_x, bottomright_y - 1);

        graphics.DrawLine(Core::Color::WHITE, topleft_x, bottomright_y, topleft_x + 1, bottomright_y);
        graphics.DrawLine(Core::Color::WHITE, topleft_x, bottomright_y, topleft_x, bottomright_y - 1);
        graphics.DrawLine(Core::Color::WHITE, bottomright_x, topleft_y, bottomright_x - 1, topleft_y);
        graphics.DrawLine(Core::Color::WHITE, bottomright_x, topleft_y, bottomright_x, topleft_y + 1);
    }
};

struct MidpointReticle : public Reticle {
    MidpointReticle() = default;
    void Draw(Core::Controller& controller, Core::Vector2 screen_point) override
    {
        Reticle::Draw(controller, screen_point);

        auto& graphics = controller.GetGraphics();
        graphics.DrawLine(
            Core::Color::GREEN, screen_point.x - 3, screen_point.y - 3, screen_point.x, screen_point.y + 3);
        graphics.DrawLine(
            Core::Color::GREEN, screen_point.x, screen_point.y + 3, screen_point.x + 3, screen_point.y - 3);
        graphics.DrawLine(
            Core::Color::GREEN, screen_point.x + 3, screen_point.y - 3, screen_point.x - 3, screen_point.y - 3);
    }
};

struct EndpointReticle : public Reticle {
    void Draw(Core::Controller& controller, Core::Vector2 screen_point) override
    {
        Reticle::Draw(controller, screen_point);

        auto& graphics = controller.GetGraphics();
        graphics.DrawRect(Core::Color::GREEN, screen_point.x - 3, screen_point.y - 3, 6, 6);
    }
};

struct IntersectionReticle : public Reticle {
    void Draw(Core::Controller& controller, Core::Vector2 screen_point) override
    {
        Reticle::Draw(controller, screen_point);

        auto& graphics = controller.GetGraphics();
        // Draw an X at the intersection point
        graphics.DrawLine(Core::Color::GREEN, screen_point.x - 3, screen_point.y - 3, screen_point.x + 3, screen_point.y + 3);
        graphics.DrawLine(Core::Color::GREEN, screen_point.x + 3, screen_point.y - 3, screen_point.x - 3, screen_point.y + 3);
            }
};

struct SnapVisitor : public ObjectVisitor {
    std::vector<SnapVector> snap_vectors;
    Core::Vector2 mouse_pos;
    Core::Transform view_transform;
    float grid_size = 1;

    SnapVisitor(Core::Vector2 mouse_pos, Core::Transform view_transform, float grid_size)
        : mouse_pos(mouse_pos), view_transform(view_transform), grid_size(grid_size)
    {
    }

    void Visit(LineObject& object) override
    {
        // Check for the midpoint snap condition
        auto start_screen = view_transform.Apply(object.start);
        auto end_screen = view_transform.Apply(object.end);
        auto midpoint_screen = (start_screen + end_screen) / 2;
        auto distance_from_midpoint = (mouse_pos - midpoint_screen).Length();
        float grid_screen_size = grid_size * view_transform.scale;
        if (distance_from_midpoint < 10) {
            snap_vectors.push_back(SnapVector(midpoint_screen, ReticleType::Midpoint));
        }
        // Check for the endpoint snap condition
        auto distance_from_start = (mouse_pos - start_screen).Length();
        if (distance_from_start < 10) {
            snap_vectors.push_back(SnapVector(start_screen, ReticleType::Endpoint));
        }

        auto distance_from_end = (mouse_pos - end_screen).Length();
        if (distance_from_end < 10) {
            snap_vectors.push_back(SnapVector(end_screen, ReticleType::Endpoint));
        }
    }

    void Visit(CircleObject& object) override
    {
        // Check for the center snap condition
    }

    void Visit(PolylineObject& object) override
    {
        // Check for the midpoint snap condition
        // Check for the endpoint snap condition
        // Check for the intersection snap condition
    }

    std::vector<SnapVector> CollectResults()
    {
        // return a vector of SnapVectors
        auto result = std::move(snap_vectors);
        snap_vectors.clear();
        return result;
    }
};

struct Ray {
    Core::Vector2 origin;
    Core::Vector2 direction;

    Ray(Core::Vector2 origin, Core::Vector2 direction) : origin(origin), direction(direction) {}

    std::optional<Core::Vector2> GetLineIntersection(Core::Vector2 start, Core::Vector2 end) {
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
};

struct RaycastSnapVisitor : public ObjectVisitor {
    std::vector<SnapVector> snap_vectors;
    Core::Vector2 mouse_pos;
    Core::Transform view_transform;
    std::vector<Ray> rays;

    RaycastSnapVisitor(std::vector<Ray>  rays, Core::Transform transform, Core::Vector2 mouse_pos) : rays(rays), view_transform(transform),
        mouse_pos(mouse_pos) {}

    void Visit(LineObject& object) override
    {
        // check for line intersection
        for (auto &ray : rays) {
        auto intersection = ray.GetLineIntersection(object.start, object.end);
        if (!intersection.has_value()) return;
        auto intersection_screen = view_transform.Apply(intersection.value());
        float distance_from_mouse = (mouse_pos - intersection_screen).Length();
        if (distance_from_mouse < 10) {
            snap_vectors.push_back(SnapVector(intersection_screen, ReticleType::Intersection));
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

    std::vector<SnapVector> CollectResults()
    {
        // return a vector of SnapVectors
        auto result = std::move(snap_vectors);
        snap_vectors.clear();
        return result;
    }
};

class Cursor {
    float x, y;
    float scale;
    std::unique_ptr<Reticle> reticle;
    bool is_grid_snapped = false;

    std::unique_ptr<Reticle> CreateReticle(ReticleType type)
    {
        switch (type) {
        case ReticleType::Grid: return std::make_unique<GridpointReticle>();
        case ReticleType::Midpoint: return std::make_unique<MidpointReticle>();
        case ReticleType::Endpoint: return std::make_unique<EndpointReticle>();
        case ReticleType::Intersection: return std::make_unique<IntersectionReticle>();
        default: return std::make_unique<GridpointReticle>(); ;
        }
    }

  public:
    Cursor() = default;
    void Update(Core::Controller& controller, ObjectRegistry& registry, Core::Transform view_transform, float grid_size,
        Ray raycast)
    {
        // Update x and y to the mouse position
        Core::Vector2 mouse_position = controller.GetInput().GetMousePosition();
        x = mouse_position.x;
        y = mouse_position.y;
        Core::Vector2 snapped_position(x, y);

        // // Using the gridpoint reticle, and snap the x and y to the grid
        std::unique_ptr<GridpointReticle> gridpoint_reticle;
        gridpoint_reticle = std::make_unique<GridpointReticle>();
        reticle = std::move(gridpoint_reticle);

        if (is_grid_snapped) {
            Core::Vector2 cursor_world = view_transform.Inverse().Apply(mouse_position);
            float grid_x = std::round(cursor_world.x / grid_size) * grid_size;
            float grid_y = std::round(cursor_world.y / grid_size) * grid_size;
            cursor_world = Core::Vector2(grid_x, grid_y);
            Core::Vector2 snapped_position = view_transform.Apply(cursor_world);
            x = snapped_position.x;
            y = snapped_position.y;
        }

        // Using the SnapVisitor, iterate over the objects and determine if the cursor is near any of the objects
        // std::vector<SnapVector> snap_vectors;
        SnapVisitor snap_visitor(Core::Vector2(x, y), view_transform, grid_size);
        registry.VisitObjects(snap_visitor);
        auto snap_vectors = snap_visitor.CollectResults();

        // Also check for raycast intersections
        RaycastSnapVisitor raycast_snap_visitor({raycast}, view_transform, Core::Vector2(x, y));
        registry.VisitObjects(raycast_snap_visitor);
        std::vector<SnapVector> raycast_snap_vectors = raycast_snap_visitor.CollectResults();
        for (auto snap_vector : raycast_snap_vectors) {
            snap_vectors.push_back(std::move(snap_vector));
        }

        // Draw the ray as purple dotted line

        auto& graphics = controller.GetGraphics();
        Core::Vector2 ray_origin_screen = view_transform.Apply(raycast.origin);
        // Find the line on the screen that passes through the ray origin and extends to the edge of the screen
        float ray_start_x = ray_origin_screen.x - raycast.direction.x * 1000;
        float ray_start_y = ray_origin_screen.y - raycast.direction.y * 1000;
        float ray_end_x = ray_origin_screen.x + raycast.direction.x * 1000;
        float ray_end_y = ray_origin_screen.y + raycast.direction.y * 1000;
        controller.GetGraphics().DrawDotted(
            Core::Color::PURPLE, ray_start_x, ray_start_y, ray_end_x, ray_end_y, 3);

        // If no possible snaps, just draw the gridpoint reticle
        if (snap_vectors.empty()) {
            reticle->Draw(controller, Core::Vector2(x, y));
            return;
        }

        // Determine the snap vector with the least distance from the mouse position
        SnapVector* closest_snap_vector = &snap_vectors[0];
        int i = 0;
        float min_distance = 0;
        for (auto& vector : snap_vectors) {
            auto distance = (snapped_position - vector.screen_point).Length();
            if (distance < min_distance || i == 0) {
                min_distance = distance;
                closest_snap_vector = &vector;
            }
            i++;
        }

        // Draw the reticle
        x = closest_snap_vector->screen_point.x;
        y = closest_snap_vector->screen_point.y;
        
        auto reticle = CreateReticle(closest_snap_vector->type);
        reticle->Draw(controller, closest_snap_vector->screen_point);
    }

    Core::Vector2 GetWorldPosition() const { return Core::Vector2(x, y); }

    Core::Vector2 GetScreenPosition() const { return Core::Vector2(x, y); }

    void SetGridSnapped(bool is_grid_snapped) { this->is_grid_snapped = is_grid_snapped; }
};

} // namespace Cad