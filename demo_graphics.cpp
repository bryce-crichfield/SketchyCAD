#include <Window.h>
#include <Engine.h>
#include <fstream>
#include <math.h>

struct GraphicsTest : Extension
{
    bool panel_toggle = false;
    Font mono_font;
    float delta = 0;
    
    ConsoleWindow t1 =
        ConsoleWindow([](Input &input) { return input.IsHeld(Key::LControl) && input.IsPressed(Key::Num1); });

    ConsoleWindow t2 =
        ConsoleWindow([](Input &input) { return input.IsHeld(Key::LControl) && input.IsPressed(Key::Num2); });

    FocusManager focus_manager = FocusManager();

    GraphicsTest() : mono_font(Font::LoadFromBin("assets/font/dogica.bin", 8))
    {
        t1.SetSize(180, 320);
        t1.SetTitle("Console 1");

        t2.SetPosition(300, 300);
        t2.SetSize(320, 180);
        t2.SetTitle("Console 2");

        focus_manager.RegisterWindow(&t1);
        focus_manager.RegisterWindow(&t2);
    }

    void OnUpdate(State state) override
    {
        state.graphics.Clear(Color::GRAY);

        FontGraphics font_graphics(state.graphics, mono_font);
        font_graphics.SetDisplayWidth(8);

        delta += state.chrono.GetDelta();
        
        Pallette pallette = Pallette();
        pallette.background = Color::BLACK;
        pallette.foreground = Color::WHITE;
        pallette.highlight = Color::RED;
        pallette.border = Color::WHITE;

        focus_manager.Update(state, font_graphics, pallette);
    }
};

int main(void)
{
    std::shared_ptr<GraphicsTest> test = std::make_shared<GraphicsTest>();
    Engine engine(1300, 1000, 600, 600);
    engine.AddExtension(test);
    engine.Launch();
}