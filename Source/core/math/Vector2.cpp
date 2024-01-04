#include <core/math/Vector2.h>

namespace Core {

Vector2::Vector2(float x, float y) {
    this->x = x;
    this->y = y;
}

Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const {
    return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(const Vector2& other) const {
    return Vector2(x * other.x, y * other.y);
}

Vector2 Vector2::operator/(const Vector2& other) const {
    return Vector2(x / other.x, y / other.y);
}

Vector2 Vector2::operator*(float scalar) const {
    return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator/(float scalar) const {
    return Vector2(x / scalar, y / scalar);
}

bool Vector2::IsInBounds(Vector2 point, Vector2 position, Vector2 size) {
    return point.x > position.x && point.x < position.x + size.x && point.y > position.y && point.y < position.y + size.y;
}

} // namespace Core