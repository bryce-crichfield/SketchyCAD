#include <core/graphics/Graphics.h>

namespace Core {
Vector2 Graphics::GetCenter() const { return Vector2(GetWidth() / 2.0f, GetHeight() / 2.0f); }

Vector2 Graphics::GetDimensions() const { return Vector2(GetWidth(), GetHeight()); }

void Graphics::PushTransform(const Transform& transform) { m_transform_stack.push(transform); }

void Graphics::PopTransform() {
    if (m_transform_stack.empty()) return;
    m_transform_stack.pop();
}
bool Graphics::IsTransformed() const { return m_is_transformed; }
void Graphics::SetTransformed(bool is_transformed) { m_is_transformed = is_transformed; }
bool Graphics::HasClip() { return m_clip_stack.size() > 0; }
void Graphics::PushClip(float x, float y, float w, float h) {
    auto position = Vector2(x, y);
    auto size = Vector2(w, h);
    auto pair = std::make_pair(position, size);
    m_clip_stack.push(pair);
}

void Graphics::PopClip() {
    if (!HasClip()) return;
    m_clip_stack.pop();
}
} // namespace Core