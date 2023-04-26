#include <Engine.h>
#include <math.h>

bool PointInsideOf(unsigned x, unsigned y, unsigned a, unsigned b, unsigned w, unsigned h)
{
    return x >= a && x <= a + w && y >= b && y <= b + h;
}

struct UserInterface
{
    Pixel background = Color::BLACK;
    Pixel foreground = Color::WHITE;
    Pixel border = Color::GRAY;
    Pixel highlight = Color::WHITE;

    bool is_vertical = false;
    unsigned x_offset = 0;
    unsigned y_offset = 0;

    State &state;

    UserInterface(State &state) : state(state)
    {
    }

    void AdvanceOffset(unsigned x, unsigned y)
    {
        if (is_vertical)
        {
            y_offset += y;
        }
        else
        {
            x_offset += x;
        }
    }

    bool Button(unsigned width, unsigned height)
    {
        bool isInside = PointInsideOf(state.input.mouse_x, state.input.mouse_y, x_offset, y_offset, width, height);
        bool isClicked = state.input.mouse_left && isInside;

        if (isClicked)
        {
            state.graphics.SetColor(highlight);
            state.graphics.FillRect(x_offset, y_offset, width, height);
        }
        else
        {
            state.graphics.SetColor(background);
            state.graphics.FillRect(x_offset, y_offset, width, height);
        }

        if (isInside)
        {
            state.graphics.SetColor(highlight);
            state.graphics.DrawRect(x_offset, y_offset, width, height);
        }
        else
        {
            state.graphics.SetColor(border);
            state.graphics.DrawRect(x_offset, y_offset, width, height);
        }
        
        AdvanceOffset(width, height);
        return isClicked;
    }
};

struct GraphicsTest : Extension
{
    GraphicsTest()
    {
    }

    void OnUpdate(State state) override
    {
        state.graphics.Clear(Color::BLACK);
        std::cout << "MouseX" << state.input.mouse_x << std::endl;
        std::cout << "MouseY" << state.input.mouse_y << std::endl;
        UserInterface ui(state);
        if (ui.Button(20, 20)) {
            std::cout << "Clicked" << std::endl;
        }

        if (ui.Button(20, 20)) {
            std::cout << "Clicked" << std::endl;
        }
    }
};

int main(void)
{
    std::shared_ptr<GraphicsTest> test = std::make_shared<GraphicsTest>();
    Engine engine(1200, 800, 100, 100);
    engine.AddExtension(test);
    engine.Launch();
}