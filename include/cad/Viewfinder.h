#pragma once

#include <cad/Cursor.h>
#include <core/Core.h>

namespace Cad {
class Viewfinder {
    std::unique_ptr<Cursor> cursor;

    float grid_size = 0.5f;
    float scale = 1.0f;
    float pan_x = 0.0f;
    float pan_y = 0.0f;
    Ray ray;

  public:
    Viewfinder();

    void Pan(int dx, int dy);
    void Zoom(int delta);
    void Zero(Core::Controller& controller);
    void Snap(float size) { grid_size = size; }

    Core::Vector2 GetCursor(Core::Controller& controller);
    void Update(Core::Controller& controller, ObjectRegistry& registry, RayBank& ray_bank);
    Core::Transform GetViewTransform();
};
} // namespace Cad