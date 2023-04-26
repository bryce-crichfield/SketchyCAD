#include <Engine.h>

#include <glad/glad.h>

Graphics::Graphics(std::shared_ptr<Image> image)
{
    m_image = image;
}

void Graphics::SetColor(const Pixel &pixel)
{
    m_color = pixel;
}

void Graphics::Clear(const Pixel &pixel)
{
    SetColor(pixel);
    m_image->Clear(m_color);
}

void Graphics::DrawPixel(unsigned x, unsigned y)
{
    m_image->SetPixel(x, y, m_color);
}

void Graphics::DrawLine(unsigned x0, unsigned y0, unsigned x1, unsigned y1)
{
    // Bresenham's line algorithm
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true)
    {
        DrawPixel(x0, y0);

        if (x0 == x1 && y0 == y1)
        {
            break;
        }

        e2 = 2 * err;

        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }

        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void Graphics::DrawRect(unsigned x, unsigned y, unsigned width, unsigned height)
{
    for (unsigned i = 0; i < width; i++)
    {
        DrawPixel(x + i, y);
        DrawPixel(x + i, y + height - 1);
    }

    for (unsigned i = 0; i < height; i++)
    {
        DrawPixel(x, y + i);
        DrawPixel(x + width - 1, y + i);
    }
}

void Graphics::DrawCircle(unsigned x, unsigned y, unsigned radius)
{
    // Draw Octants
    int x0 = radius;
    int y0 = 0;
    int err = 0;

    while (x0 >= y0)
    {
        DrawPixel(x + x0, y + y0);
        DrawPixel(x + y0, y + x0);
        DrawPixel(x - y0, y + x0);
        DrawPixel(x - x0, y + y0);
        DrawPixel(x - x0, y - y0);
        DrawPixel(x - y0, y - x0);
        DrawPixel(x + y0, y - x0);
        DrawPixel(x + x0, y - y0);

        y0++;
        err += 1 + 2 * y0;

        if (2 * (err - x0) + 1 > 0)
        {
            x0--;
            err += 1 - 2 * x0;
        }
    }
}

void Graphics::FillRect(unsigned x, unsigned y, unsigned width, unsigned height)
{
    for (unsigned i = 0; i < width; i++)
    {
        for (unsigned j = 0; j < height; j++)
        {
            DrawPixel(x + i, y + j);
        }
    }
}

void Graphics::FillCircle(unsigned x, unsigned y, unsigned radius)
{
    // Draw Octants
    int x0 = radius;
    int y0 = 0;
    int err = 0;

    while (x0 >= y0)
    {
        DrawLine(x - x0, y + y0, x + x0, y + y0);
        DrawLine(x - y0, y + x0, x + y0, y + x0);
        DrawLine(x - x0, y - y0, x + x0, y - y0);
        DrawLine(x - y0, y - x0, x + y0, y - x0);

        y0++;
        err += 1 + 2 * y0;

        if (2 * (err - x0) + 1 > 0)
        {
            x0--;
            err += 1 - 2 * x0;
        }
    }
}

void Graphics::DrawImage(const Image &image, unsigned x, unsigned y, unsigned w, unsigned h)
{
    for (unsigned i = 0; i < w; i++)
    {
        for (unsigned j = 0; j < h; j++)
        {
            float u = (float)i / (float)w;
            float v = (float)j / (float)h;
            SetColor(image.SamplePixel(u, v));
            DrawPixel(x + i, y + j);
        }
    }
}

unsigned Graphics::GetWidth() const
{
    return m_image->GetWidth();
}

unsigned Graphics::GetHeight() const
{
    return m_image->GetHeight();
}