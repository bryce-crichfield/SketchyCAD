#pragma once

#include <Engine.h>

namespace Cad {

struct Viewfinder {
    virtual void Pan(int dx, int dy) = 0;
    virtual void Zoom(int delta) = 0;
    virtual void Zero(Graphics& graphics) = 0;
    virtual Vector2 GetCursor(Input& input) = 0;
    virtual void OnRender(Graphics& graphics) = 0;
    virtual Transform GetViewTransform() = 0;
};

class SnapViewfinder : public Viewfinder {
    float grid_size = 0.5f;
    float scale = 1.0f;
    float pan_x = 0.0f;
    float pan_y = 0.0f;

  public:
    SnapViewfinder();

    void Pan(int dx, int dy) override;
    void Zoom(int delta) override;
    void Zero(Graphics& graphics) override;

    Vector2 GetCursor(Input& input) override;
    void OnRender(Graphics& graphics) override;
    Transform GetViewTransform() override;
};
} // namespace Cad