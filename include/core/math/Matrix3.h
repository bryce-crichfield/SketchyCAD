#pragma once

#include <core/math/Vector2.h>

namespace Core {

struct Matrix3 {
    float data[9];

    Matrix3();

    Matrix3 operator*(const Matrix3& other) const;
    Vector2 operator*(const Vector2& other) const;

    static Matrix3 Identity();
    static Matrix3 Translation(float x, float y);
    static Matrix3 Rotation(float angle);
    static Matrix3 Scale(float x, float y);
};
    
} // namespace Core