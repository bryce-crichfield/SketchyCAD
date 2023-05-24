#pragma once

#include <cad/Ray.h>
#include <cad/Reticle.h>
#include <cad/object/Object.h>
#include <cad/object/ObjectRegistry.h>
#include <cad/object/ObjectVisitor.h>
#include <core/Core.h>

namespace Cad {
struct SnapVector {
    Core::Vector2 screen_point;
    ReticleType type;

    SnapVector(Core::Vector2 screen_point, ReticleType type) : screen_point(screen_point), type(type) {}
};
class Cursor {
    float x, y;
    float scale;
    std::unique_ptr<Reticle> reticle;
    bool is_grid_snapped = false;

    std::unique_ptr<Reticle> create_reticle_from_type(ReticleType type);
    void update_mouse_location(Core::Controller& controller);
    void snap_cursor_to_grid(Core::Controller& controller, Core::Transform view_transform, float grid_size);
    void collect_snap_vectors(std::vector<SnapVector>& snap_vectors, ObjectRegistry& registry,
        Core::Transform view_transform, float grid_size, Ray raycast);
    SnapVector get_closest_snap_vector(std::vector<SnapVector>& snap_vectors, Core::Vector2 mouse_position);

  public:
    Cursor() = default;
    void Update(Core::Controller& controller, ObjectRegistry& registry, Core::Transform view_transform, float grid_size,
        Ray raycast);

    Core::Vector2 GetWorldPosition() const { return Core::Vector2(x, y); }
    Core::Vector2 GetScreenPosition() const { return Core::Vector2(x, y); }
    void SetGridSnapped(bool is_grid_snapped) { this->is_grid_snapped = is_grid_snapped; }
};

} // namespace Cad