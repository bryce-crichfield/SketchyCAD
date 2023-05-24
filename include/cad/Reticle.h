#pragma once

#include <core/Controller.h>

namespace Cad {
struct Reticle {
    virtual ~Reticle() = default;
    virtual void Draw(Core::Controller& controller, Core::Vector2 screen_point);
};

enum class ReticleType {
    Grid,
    Midpoint,
    Endpoint,
    Intersection,
    None
};

struct GridpointReticle : public Reticle {
    GridpointReticle() = default;
    void Draw(Core::Controller& controller, Core::Vector2 screen_point) override;
};

struct MidpointReticle : public Reticle {
    MidpointReticle() = default;
    void Draw(Core::Controller& controller, Core::Vector2 screen_point) override;
};

struct EndpointReticle : public Reticle {
    void Draw(Core::Controller& controller, Core::Vector2 screen_point) override;
};

struct IntersectionReticle : public Reticle {
    void Draw(Core::Controller& controller, Core::Vector2 screen_point) override;
};
} // namespace Cad