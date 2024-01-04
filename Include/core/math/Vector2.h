#pragma once

#include <cmath>
#include <string>

namespace Core {

struct Vector2 {
    float x, y = 0;

    Vector2() = default;
    Vector2(float x, float y);

    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(const Vector2& other) const;
    Vector2 operator/(const Vector2& other) const;
    Vector2 operator+(float scalar) const {
        return Vector2(x + scalar, y + scalar);
    }
    Vector2 operator-(float scalar) const {
        return Vector2(x - scalar, y - scalar);
    }
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;
    Vector2 operator+=(Vector2 other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    bool operator==(const Vector2& other) const {
        return x == other.x && y == other.y;
    }
    Vector2 Rotate(float angle) const
    {
        return Vector2(x * cos(angle) - y * sin(angle), x * sin(angle) + y * cos(angle));
    }
    float Length() const { return sqrt(x * x + y * y); }
    Vector2 ClosestPointOnLine(Vector2 line_start, Vector2 line_end) const
    {
        Vector2 line = line_end - line_start;
        float line_length = line.Length();
        Vector2 line_direction = line / line_length;
        Vector2 point = *this - line_start;
        float dot = point.x * line_direction.x + point.y * line_direction.y;
        dot = std::max(0.0f, std::min(dot, line_length));
        return line_start + line_direction * dot;
    }
    float Dot(Vector2 other) const { return x * other.x + y * other.y; }
    float Cross(Vector2 other) const { return x * other.y - y * other.x; }
    static bool IsInBounds(Vector2 point, Vector2 position, Vector2 size);

    static Vector2 FromAngle(float angle) { return Vector2(cos(angle), sin(angle)); }

    Vector2 Normalized() const { return *this / Length(); }
    float Angle() const { 
        float dot = Dot(Vector2(1, 0));
        float angle = acos(dot / Length());
        if (y < 0) {
            angle = -angle;
        }
        return angle;
    }

    float Angle(Vector2 other) const { 
        float dot = Dot(other);
        float mag1 = Length();
        float mag2 = other.Length();
        float cosTheta = dot / (mag1 * mag2);
        float thetaRad = acos(cosTheta);
        return thetaRad;
    }
    Vector2 Rotate(float angle, Vector2 origin) const { return (*this - origin).Rotate(angle) + origin; }
    std::string ToString() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

} // namespace Core