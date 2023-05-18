#include <Engine.h>

Transform::Transform(float x, float y, float scale, float rotation) {
    this->x = x;
    this->y = y;
    this->scale = scale;
    this->rotation = rotation;
}

Transform::Transform() {
    this->x = 0;
    this->y = 0;
    this->scale = 1;
    this->rotation = 0;
}

Transform Transform::Identity() {
    return Transform();
}

Transform& Transform::Translate(float x, float y) {
    this->x += x;
    this->y += y;
    return *this;
}

Transform& Transform::Scale(float scale) {
    this->scale *= scale;
    return *this;
}

Transform& Transform::Rotate(float rotation) {
    this->rotation += rotation;
    return *this;
}

Vector2 Transform::Apply(Vector2 point) const {
    // TODO: Add rotation
    float x = point.x * scale + this->x;
    float y = point.y * scale + this->y;
    return Vector2(x, y);
}

Transform Transform::Inverse() const {
    float x = -this->x / scale;
    float y = -this->y / scale;
    float scale = 1 / this->scale;
    return Transform(x, y, scale, 0);
}