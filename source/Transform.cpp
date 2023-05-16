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

    return Vector2(point.x * scale + x, point.y * scale + y);
}