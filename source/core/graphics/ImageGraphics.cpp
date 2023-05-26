
#include <core/graphics/ImageGraphics.h>

namespace Core {

ImageGraphics::ImageGraphics(Image& image) : Graphics(), m_image(image) {}

void ImageGraphics::Clear(Pixel color) { m_image.Clear(color); }

void ImageGraphics::SetPixel(Pixel color, unsigned x, unsigned y) {
    if (!HasClip()) {
        m_image.SetPixel(x, y, color);
        return;
    }
    auto top_clip = m_clip_stack.top();
    if (x < top_clip.first.x) return;
    if (y < top_clip.first.y) return;
    if (x > top_clip.first.x + top_clip.second.x) return;
    if (y > top_clip.first.y + top_clip.second.y) return;
    m_image.SetPixel(x, y, color);
}

void ImageGraphics::DrawLine(Pixel color, float x0_in, float y0_in, float x1_in, float y1_in) {
    auto transform = IsTransformed() && !m_transform_stack.empty() ? m_transform_stack.top() : Transform::Identity();
    Vector2 p0 = transform.Apply(Vector2(x0_in, y0_in));
    Vector2 p1 = transform.Apply(Vector2(x1_in, y1_in));

    int x0 = p0.x;
    int y0 = p0.y;
    int x1 = p1.x;
    int y1 = p1.y;

    // Bresenham's line algorithm
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true) {
        SetPixel(color, x0, y0);

        if (x0 == x1 && y0 == y1) {
            break;
        }

        e2 = 2 * err;

        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }

        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void ImageGraphics::DrawDotted(Pixel color, float x0_in, float y0_in, float x1_in, float y1_in, float width) {
    // Draw a line using the fast delta method
    auto transform = IsTransformed() && !m_transform_stack.empty() ? m_transform_stack.top() : Transform::Identity();
    Vector2 p0 = transform.Apply(Vector2(x0_in, y0_in));
    Vector2 p1 = transform.Apply(Vector2(x1_in, y1_in));

    int x0 = p0.x;
    int y0 = p0.y;
    int x1 = p1.x;
    int y1 = p1.y;

    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;
    int dot_index = 0;
    bool dot = true;
    while (true) {
        if (dot) {
            SetPixel(color, x0, y0);
        }
        dot_index++;
        if (dot_index == width) {
            dot_index = 0;
            dot = !dot;
        }

        if (x0 == x1 && y0 == y1) {
            break;
        }

        e2 = 2 * err;

        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }

        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void ImageGraphics::DrawRect(Pixel color, unsigned x_in, unsigned y_in, unsigned width_in, unsigned height_in) {
    auto transform = IsTransformed() && !m_transform_stack.empty() ? m_transform_stack.top() : Transform::Identity();
    Vector2 position = transform.Apply(Vector2(x_in, y_in));
    float x = position.x;
    float y = position.y;
    float width = width_in * transform.GetScale();
    float height = height_in * transform.GetScale();

    for (unsigned i = 0; i < width; i++) {
        SetPixel(color, x + i, y);
        SetPixel(color, x + i, y + height - 1);
    }

    for (unsigned i = 0; i < height; i++) {
        SetPixel(color, x, y + i);
        SetPixel(color, x + width - 1, y + i);
    }
}

void ImageGraphics::DrawCircle(Pixel color, float x_in, float y_in, float radius_in) {
    auto transform = IsTransformed() && !m_transform_stack.empty() ? m_transform_stack.top() : Transform::Identity();
    Vector2 position = transform.Apply(Vector2(x_in, y_in));
    float x = position.x;
    float y = position.y;
    float radius = radius_in * transform.GetScale();

    // Draw Octants
    int x0 = radius;
    int y0 = 0;
    int err = 0;

    while (x0 >= y0) {
        SetPixel(color, x + x0, y + y0);
        SetPixel(color, x + y0, y + x0);
        SetPixel(color, x - y0, y + x0);
        SetPixel(color, x - x0, y + y0);
        SetPixel(color, x - x0, y - y0);
        SetPixel(color, x - y0, y - x0);
        SetPixel(color, x + y0, y - x0);
        SetPixel(color, x + x0, y - y0);

        y0++;
        err += 1 + 2 * y0;

        if (2 * (err - x0) + 1 > 0) {
            x0--;
            err += 1 - 2 * x0;
        }
    }
}

void ImageGraphics::DrawArc(Pixel color, float x, float y, float radius, float start_angle, float end_angle) {
    auto transform = IsTransformed() && !m_transform_stack.empty() ? m_transform_stack.top() : Transform::Identity();
    auto origin = Vector2(x, y);
    auto screen_origin = transform.Apply(Vector2(x, y));
    auto screen_radius = radius * transform.GetScale();

    // sort angles
    if (start_angle > end_angle) {
        float tmp = start_angle;
        start_angle = end_angle;
        end_angle = tmp;
    }


    float angle = start_angle;
    float step = 0.01f;
    while (angle < end_angle) {
        float next_angle = angle + step;
        if (next_angle > end_angle) {
            next_angle = end_angle;
        }

        auto world_start = origin + (Vector2::FromAngle(angle) * radius);
        auto world_end = origin + (Vector2::FromAngle(next_angle) * radius);
        
        auto screen_start = transform.Apply(world_start);
        auto screen_end = transform.Apply(world_end);

        bool is_transformed = IsTransformed();
        SetTransformed(false);
        DrawLine(color, screen_start.x, screen_start.y, screen_end.x, screen_end.y);
        SetTransformed(is_transformed);

        angle = next_angle;
    }
}

void ImageGraphics::FillRect(Pixel color, unsigned x_in, unsigned y_in, unsigned w_in, unsigned h_in) {
    auto transform = IsTransformed() && !m_transform_stack.empty() ? m_transform_stack.top() : Transform::Identity();
    Vector2 position = transform.Apply(Vector2(x_in, y_in));
    float x = position.x;
    float y = position.y;
    float w = w_in * transform.GetScale();
    float h = h_in * transform.GetScale();

    for (unsigned i = 0; i < w; i++) {
        for (unsigned j = 0; j < h; j++) {
            SetPixel(color, x + i, y + j);
        }
    }
}

void ImageGraphics::FillCircle(Pixel color, unsigned x_in, unsigned y_in, unsigned radius_in) {
    auto transform = IsTransformed() && !m_transform_stack.empty() ? m_transform_stack.top() : Transform::Identity();
    Vector2 position = transform.Apply(Vector2(x_in, y_in));
    float x = position.x;
    float y = position.y;
    float radius = radius_in * transform.GetScale();

    // Draw Octants
    int x0 = radius;
    int y0 = 0;
    int err = 0;

    while (x0 >= y0) {
        DrawLine(color, x - x0, y + y0, x + x0, y + y0);
        DrawLine(color, x - y0, y + x0, x + y0, y + x0);
        DrawLine(color, x - x0, y - y0, x + x0, y - y0);
        DrawLine(color, x - y0, y - x0, x + y0, y - x0);

        y0++;
        err += 1 + 2 * y0;

        if (2 * (err - x0) + 1 > 0) {
            x0--;
            err += 1 - 2 * x0;
        }
    }
}

unsigned ImageGraphics::GetWidth() const { return m_image.GetWidth(); }

unsigned ImageGraphics::GetHeight() const { return m_image.GetHeight(); }
} // namespace Core