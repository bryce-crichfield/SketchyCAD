#pragma once

#include <core/math/Vector2.h>

namespace Core {

struct Transform {
    float x, y, scale, rotation;

    Transform(float x, float y, float scale, float rotation);
    Transform();

    static Transform Identity();
    Transform& Translate(float x, float y);
    Transform& Scale(float scale);
    Transform& Rotate(float rotation);

    Vector2 Apply(Vector2 point) const;
    Transform Inverse() const;

    float GetX() const { return x; }
    float GetY() const { return y; }
    float GetScale() const { return scale; }
    float GetRotation() const { return rotation; }
};

} // namespace Core