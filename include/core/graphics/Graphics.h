#pragma once

#include <core/math/Transform.h>
#include <core/math/Vector2.h>
#include <core/graphics/Pixel.h>

#include <stack>

namespace Core {
class Graphics {
    std::stack<Transform> m_transform_stack;

  public:
    Graphics() = default;
    virtual ~Graphics() = default;

    Vector2 GetCenter() const;
    Vector2 GetDimensions() const;

    void PushTransform(const Transform& transform);
    Transform PopTransform();
    Transform GetTransform() const;
    Vector2 TransformPoint(const Vector2& point) const;

    virtual unsigned GetWidth() const = 0;
    virtual unsigned GetHeight() const = 0;

    virtual void Clear(Pixel color = Color::BLACK) = 0;
    virtual void SetPixel(Pixel color, unsigned x, unsigned y) = 0;
    virtual void DrawLine(Pixel color, float x0, float y0, float x1, float y1) = 0;
    virtual void DrawRect(Pixel color, unsigned x, unsigned y, unsigned width, unsigned height) = 0;
    virtual void DrawCircle(Pixel color, float x, float y, float radius) = 0;
    virtual void DrawTriangle(Pixel color, float x0, float y0, float x1, float y1, float x2, float y2) = 0;
    virtual void FillRect(Pixel color, unsigned x, unsigned y, unsigned width, unsigned height) = 0;
    virtual void FillCircle(Pixel color, unsigned x, unsigned y, unsigned radius) = 0;
};
} // namespace Core