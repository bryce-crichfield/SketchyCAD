#include <core/graphics/Graphics.h>

namespace Core {
Vector2 Graphics::GetCenter() const { return Vector2(GetWidth() / 2.0f, GetHeight() / 2.0f); }

Vector2 Graphics::GetDimensions() const { return Vector2(GetWidth(), GetHeight()); }

void Graphics::PushTransform(const Transform& transform) { m_transform_stack.push(transform); }

Transform Graphics::PopTransform()
{
    if (m_transform_stack.empty()) return Transform::Identity();
    Transform transform = m_transform_stack.top();
    m_transform_stack.pop();
    return transform;
}

Transform Graphics::GetTransform() const
{
    if (m_transform_stack.empty()) return Transform::Identity();
    return m_transform_stack.top();
}
Vector2 Graphics::TransformPoint(const Vector2& point) const
{
    auto transform = GetTransform();
    Vector2 result = transform.Apply(point);
    return result;
}
} // namespace Core