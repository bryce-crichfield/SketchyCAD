#pragma once

#include <core/graphics/Graphics.h>
#include <core/graphics/Image.h>

#include <memory>
#include <stack>

namespace Core {
class ImageGraphics : public Graphics {
    Image& m_image;

  public:
  // SUSSY! should be shared_ptr but it wasn't working :{
    ImageGraphics(Image& image);
    ~ImageGraphics() = default;


    unsigned GetWidth() const override;
    unsigned GetHeight() const override;

    void Clear(Pixel color = Color::BLACK) override;
    void SetPixel(Pixel color, unsigned x, unsigned y) override;
    void DrawLine(Pixel color, float x0, float y0, float x1, float y1) override;
    void DrawDotted(Pixel color, float x0, float y0, float x1, float y1, float width) override;

    void DrawRect(Pixel color, unsigned x, unsigned y, unsigned width, unsigned height) override;
    void DrawCircle(Pixel color, float x, float y, float radius) override;
    void DrawTriangle(Pixel color, float x0, float y0, float x1, float y1, float x2, float y2) override
    {
        DrawLine(color, x0, y0, x1, y1);
        DrawLine(color, x1, y1, x2, y2);
        DrawLine(color, x2, y2, x0, y0);
    }
    void FillRect(Pixel color, unsigned x, unsigned y, unsigned width, unsigned height) override;
    void FillCircle(Pixel color, unsigned x, unsigned y, unsigned radius) override;
};
} // namespace Core