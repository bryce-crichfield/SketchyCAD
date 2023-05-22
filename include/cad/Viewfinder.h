#pragma once

#include <core/Core.h>
#include <cad/Cursor.h>

namespace Cad {

struct Reticle;
struct Viewfinder {
    std::unique_ptr<Cursor> cursor;
    virtual void Pan(int dx, int dy) = 0;
    virtual void Zoom(int delta) = 0;
    virtual void Zero(Core::Controller& controller) = 0;
    virtual Core::Vector2 GetCursor(Core::Controller& controller) = 0;
    virtual void Update(Core::Controller& controller, ObjectRegistry& registry) = 0;
    virtual Core::Transform GetViewTransform() = 0;

    // TODO: This isn't gonna work with polymorphism
    virtual void Snap(float size) = 0;
};

class SnapViewfinder : public Viewfinder {
    float grid_size = 0.5f;
    float scale = 1.0f;
    float pan_x = 0.0f;
    float pan_y = 0.0f;
    Ray ray;
  public:
    SnapViewfinder();

    void Pan(int dx, int dy) override;
    void Zoom(int delta) override;
    void Zero(Core::Controller& controller) override;
    void Snap(float size) override { grid_size = size; }

    Core::Vector2 GetCursor(Core::Controller& controller) override;
    void Update(Core::Controller& controller,  ObjectRegistry& registry) override;
    Core::Transform GetViewTransform() override;
};
} // namespace Cad