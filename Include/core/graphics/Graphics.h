#pragma once

#include <core/graphics/Pixel.h>
#include <core/graphics/Image.h>
#include <core/math/Transform.h>
#include <core/math/Vector2.h>

#include <stack>

namespace Core {
struct Image;
class Graphics {
  protected:
    bool m_is_transformed = true;
    std::stack<Transform> m_transform_stack;
    std::stack<std::pair<Core::Vector2, Core::Vector2>> m_clip_stack;

  public:
    Graphics() = default;
    virtual ~Graphics() = default;

    Vector2 GetCenter() const;
    Vector2 GetDimensions() const;

    bool IsTransformed() const;
    void SetTransformed(bool is_transformed);
    void PushTransform(const Transform& transform);
    void PopTransform();

    bool HasClip();
    void PushClip(float x, float y, float w, float h);
    void PopClip();

    virtual unsigned GetWidth() const = 0;
    virtual unsigned GetHeight() const = 0;

    virtual void Clear(Pixel color = Color::BLACK) = 0;
    virtual void SetPixel(Pixel color, unsigned x, unsigned y) = 0;
    virtual void DrawLine(Pixel color, float x0, float y0, float x1, float y1) = 0;
    virtual void DrawDotted(Pixel color, float x0, float y0, float x1, float y1, float width) = 0;
    virtual void DrawRect(Pixel color, unsigned x, unsigned y, unsigned width, unsigned height) = 0;
    virtual void DrawCircle(Pixel color, float x, float y, float radius) = 0;
    virtual void DrawTriangle(Pixel color, float x0, float y0, float x1, float y1, float x2, float y2) = 0;
    virtual void DrawArc(Pixel color, float x, float y, float radius, float start_angle, float end_angle) = 0;
    virtual void DrawImage(Image& image, float x, float y, float width, float height) = 0;
    virtual void FillRect(Pixel color, unsigned x, unsigned y, unsigned width, unsigned height) = 0;
    virtual void FillCircle(Pixel color, unsigned x, unsigned y, unsigned radius) = 0;
    virtual void FillTriangle(Pixel color, float x0, float y0, float x1, float y1, float x2, float y2) = 0;
};
} // namespace Core