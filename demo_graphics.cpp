#include <Console.h>
#include <Engine.h>
#include <fstream>
#include <math.h>

struct TransformGraphics
{
    float rotation = 0;
    float scale = 1;
    float x = 0;
    float y = 0;

    Graphics &graphics;

    TransformGraphics(Graphics &graphics) : graphics(graphics)
    {
    }

    void DrawImage(Image &image)
    {
        unsigned draw_width = image.GetWidth() * scale;
        unsigned draw_height = image.GetHeight() * scale;
        float cos_theta = cos(rotation);
        float sin_theta = sin(rotation);

        for (int dx = 0; dx < draw_width; dx++) {
            for (int dy = 0; dy < draw_height; dy++) {
                float u = (float)dx / (float)draw_width;
                float v = (float)dy / (float)draw_height;
                float draw_x = dx * cos_theta - dy * sin_theta;
                float draw_y = dx * sin_theta + dy * cos_theta;
                draw_x += this->x;
                draw_y += this->y;
                Pixel color = image.SamplePixel(u, v);
                graphics.DrawPixel(color, draw_x, draw_y);
            }
        }
    }
};

struct GraphicsTest : Extension
{
    ConsolePanel console_panel = ConsolePanel(50, 50, 300, 400);
    bool panel_toggle = false;
    Font mono_font;
    float delta = 0;
    GraphicsTest() : mono_font(Font::LoadFromBin("assets/font/dogica.bin", 8))
    {
    }

    void OnUpdate(State state) override
    {
        state.graphics.Clear(Color::GRAY);

        FontGraphics font_graphics(state.graphics, mono_font);
        font_graphics.SetDisplayWidth(8);

        bool ctrl_tilde = state.input.IsHeld(Key::LControl) && state.input.IsPressed(Key::Tilde);
        if (ctrl_tilde)
        {
            panel_toggle = !panel_toggle;
        }

        if (panel_toggle)
        {
            console_panel.OnUpdate(state, font_graphics);
        }

        TransformGraphics transform_graphics(state.graphics);
        delta += state.chrono.GetDelta();
        transform_graphics.rotation = delta;
        transform_graphics.scale = 0.5;
        transform_graphics.x = 100;
        transform_graphics.y = 100;
        std::shared_ptr<Image> image = std::make_shared<Image>(100, 100);
        Graphics image_graphics(image);
        image_graphics.Clear(Color::RED);
        image_graphics.FillRect(Color::BLUE, 10, 10, 80, 80);
        transform_graphics.DrawImage(*image);
    }
};

int main(void)
{
    std::shared_ptr<GraphicsTest> test = std::make_shared<GraphicsTest>();
    Engine engine(1300, 1000, 300, 300);
    engine.AddExtension(test);
    engine.Launch();
}