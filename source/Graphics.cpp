#include <Engine.h>

#include <glad/glad.h>

Graphics::Graphics(std::shared_ptr<Image> image)
{
    m_image = image;
}

void Graphics::Clear(Pixel color)
{
    m_image->Clear(color);
}

void Graphics::DrawPixel(Pixel color, unsigned x, unsigned y)
{
    m_image->SetPixel(x, y, color);
}

void Graphics::DrawLine(Pixel color, unsigned x0, unsigned y0, unsigned x1, unsigned y1)
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
        DrawPixel(color, x0, y0);

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

void Graphics::DrawRect(Pixel color, unsigned x, unsigned y, unsigned width, unsigned height)
{
    for (unsigned i = 0; i < width; i++)
    {
        DrawPixel(color, x + i, y);
        DrawPixel(color, x + i, y + height - 1);
    }

    for (unsigned i = 0; i < height; i++)
    {
        DrawPixel(color, x, y + i);
        DrawPixel(color, x + width - 1, y + i);
    }
}

void Graphics::DrawCircle(Pixel color, unsigned x, unsigned y, unsigned radius)
{
    // Draw Octants
    int x0 = radius;
    int y0 = 0;
    int err = 0;

    while (x0 >= y0)
    {
        DrawPixel(color, x + x0, y + y0);
        DrawPixel(color, x + y0, y + x0);
        DrawPixel(color, x - y0, y + x0);
        DrawPixel(color, x - x0, y + y0);
        DrawPixel(color, x - x0, y - y0);
        DrawPixel(color, x - y0, y - x0);
        DrawPixel(color, x + y0, y - x0);
        DrawPixel(color, x + x0, y - y0);

        y0++;
        err += 1 + 2 * y0;

        if (2 * (err - x0) + 1 > 0)
        {
            x0--;
            err += 1 - 2 * x0;
        }
    }
}

void Graphics::FillRect(Pixel color, unsigned x, unsigned y, unsigned width, unsigned height)
{
    for (unsigned i = 0; i < width; i++)
    {
        for (unsigned j = 0; j < height; j++)
        {
            DrawPixel(color, x + i, y + j);
        }
    }
}

void Graphics::FillCircle(Pixel color, unsigned x, unsigned y, unsigned radius)
{
    // Draw Octants
    int x0 = radius;
    int y0 = 0;
    int err = 0;

    while (x0 >= y0)
    {
        DrawLine(color, x - x0, y + y0, x + x0, y + y0);
        DrawLine(color, x - y0, y + x0, x + y0, y + x0);
        DrawLine(color, x - x0, y - y0, x + x0, y - y0);
        DrawLine(color, x - y0, y - x0, x + y0, y - x0);

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
            Pixel color = image.SamplePixel(u, v);
            DrawPixel(color, x + i, y + j);
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