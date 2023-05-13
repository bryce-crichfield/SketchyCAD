#include <Engine.h>
#include <Window.h>
#include <climits>
#include <fstream>
#include <math.h>

struct DebouncedKey
{
    Debouncer debouncer;
    PulseTimer wait_timer;

  public:
    DebouncedKey(Key key, float time) : debouncer(key), wait_timer(time)
    {
    }

    bool Update(State state)
    {
        auto &input = state.input;
        auto &chrono = state.chrono;

        debouncer.Update(input, chrono);
        bool is_up_time = wait_timer.Update(chrono);
        if (debouncer.IsHeld() && is_up_time)
        {
            wait_timer.Reset();
            return true;
        }

        return input.IsPressed(debouncer.matching_key);
    }
};

struct Camera
{
    int pan_x = 250;
    int pan_y = 250;
    float scale = 10;

    DebouncedKey up_key = DebouncedKey(Key::Up, 0.325);
    DebouncedKey down_key = DebouncedKey(Key::Down, 0.325);
    DebouncedKey left_key = DebouncedKey(Key::Left, 0.325);
    DebouncedKey right_key = DebouncedKey(Key::Right, 0.325);

  public:
    void Update(State state)
    {
        auto &input = state.input;
        auto &chrono = state.chrono;
        bool is_drag =
            input.IsHeld(MouseButton::MIDDLE) && (input.GetMouseDeltaX() != 0 || input.GetMouseDeltaY() != 0);

        if (is_drag)
        {
            pan_x += input.GetMouseDeltaX();
            pan_y += input.GetMouseDeltaY();
        }

        if (up_key.Update(state))
        {
            pan_y += 10 * scale;
        }

        if (down_key.Update(state))
        {
            pan_y -= 10 * scale;
        }

        if (left_key.Update(state))
        {
            pan_x += 10 * scale;
        }

        if (right_key.Update(state))
        {
            pan_x -= 10 * scale;
        }

        scale += 50 * input.GetScrollDeltaY() * chrono.GetDelta();
        scale = std::clamp(scale, 0.5f, 10.0f);
        // snap scale to nearest multiple of 0.01
        scale = roundf(scale * 100) / 100;
    }

    Transform GetTransform()
    {
        Transform transform;
        transform.Translate(pan_x, pan_y);
        transform.Scale(scale);
        return transform;
    }
};

struct ICadObject
{
    virtual void Draw(Graphics &graphics) = 0;
};

struct LineObject : ICadObject
{
    Vector2 start;
    Vector2 end;

    LineObject(Vector2 start, Vector2 end) : start(start), end(end)
    {
    }

    void Draw(Graphics &graphics)
    {
        graphics.DrawLine(Color::WHITE, start.x, start.y, end.x, end.y);
    }
};

struct CircleObject : ICadObject
{
    Vector2 center;
    float radius;

    CircleObject(Vector2 center, float radius) : center(center), radius(radius)
    {
    }

    void Draw(Graphics &graphics)
    {
        graphics.DrawCircle(Color::WHITE, center.x, center.y, radius);
    }
};

struct CadController;

struct ICadMode
{
    virtual void Update(CadController &state, Input &input) = 0;
    std::string GetName() { return typeid(*this).name(); }
};

struct CadController
{
    std::vector<std::unique_ptr<ICadObject>> objects;
    Camera camera;
    std::unique_ptr<ICadMode> current_mode = nullptr;

    void Update(State state);
};

struct LineMode : ICadMode
{
    std::stack<Vector2> points;

    LineMode() : points()
    {
    }

    void Update(CadController &state, Input &input) override
    {
        if (points.size() == 2)
        {
            Vector2 start = points.top();
            points.pop();
            Vector2 end = points.top();
            points.pop();

            state.objects.push_back(std::make_unique<LineObject>(start, end));
        }

        if (input.IsPressed(MouseButton::LEFT))
        {
            Transform view_transform = state.camera.GetTransform();
            float mouse_x = input.GetMouseX();
            float mouse_y = input.GetMouseY();
            float world_x = (mouse_x - view_transform.x) / view_transform.scale;
            float world_y = (mouse_y - view_transform.y) / view_transform.scale;
            Vector2 world_pos = Vector2(world_x, world_y);
            points.push(world_pos);
        }
    }
};

struct CircleMode : ICadMode
{
    std::stack<Vector2> points;

    CircleMode() : points()
    {
    }

    void Update(CadController &state, Input &input) override
    {
        if (points.size() == 2)
        {
            Vector2 radius = points.top();
            points.pop();
            Vector2 center = points.top();
            points.pop();

            float r = (center - radius).Length();
            state.objects.push_back(std::make_unique<CircleObject>(center, r));
        }

        if (input.IsPressed(MouseButton::LEFT))
        {
            Transform view_transform = state.camera.GetTransform();
            float mouse_x = input.GetMouseX();
            float mouse_y = input.GetMouseY();
            float world_x = (mouse_x - view_transform.x) / view_transform.scale;
            float world_y = (mouse_y - view_transform.y) / view_transform.scale;
            Vector2 world_pos = Vector2(world_x, world_y);
            std::cout << "world_pos: " << world_pos.x << ", " << world_pos.y << std::endl;
            points.push(world_pos);
        }
    }
};

void CadController::Update(State state)
{
    camera.Update(state);

    if (state.input.IsPressed(Key::L))
    {
        current_mode = std::make_unique<LineMode>();
    }
    else if (state.input.IsPressed(Key::C))
    {
        current_mode = std::make_unique<CircleMode>();
    }

    if (current_mode != nullptr)
    {
        current_mode->Update(*this, state.input);
    }

    state.graphics.PushTransform(camera.GetTransform());
    for (auto &object : objects)
    {
        object->Draw(state.graphics);
    }
    state.graphics.PopTransform();
}

struct CadWorld
{
    CadWorld()
    {
    }

    void Update(State state, Camera &camera)
    {
        Transform view_transform = camera.GetTransform();

        float scale = view_transform.scale;
        unsigned width = state.graphics.GetWidth();
        unsigned height = state.graphics.GetHeight();

        state.graphics.PushTransform(view_transform);
        // Draw World
        state.graphics.PopTransform();

        // Grid Lines -------------------------------------------
        {
            unsigned pan_x = view_transform.x + (scale / view_transform.x);
            unsigned pan_y = view_transform.y + (scale / view_transform.y);

            unsigned grid_size = 10 * scale;

            unsigned start_x = pan_x % grid_size;
            unsigned start_y = pan_y % grid_size;

            for (unsigned x = start_x; x < width; x += grid_size)
            {
                for (unsigned y = start_y; y < height; y += grid_size)
                {
                    state.graphics.SetPixel(Color::GRAY, x, y);
                }
            }
        }

        // Draw Origin Lines
        float pan_x = view_transform.x + (scale / view_transform.x);
        float pan_y = view_transform.y + (scale / view_transform.y);
        state.graphics.DrawLine(Color::RED, pan_x, 0, pan_x, state.graphics.GetHeight());
        state.graphics.DrawLine(Color::GREEN, 0, pan_y, state.graphics.GetWidth(), pan_y);

        // Draw Cursor Lines
        float cursor_x = state.input.GetMouseX();
        float cursor_y = state.input.GetMouseY();
        state.graphics.DrawLine(Color::WHITE, cursor_x, 0, cursor_x, state.graphics.GetHeight());
        state.graphics.DrawLine(Color::WHITE, 0, cursor_y, state.graphics.GetWidth(), cursor_y);
        float world_x = (cursor_x - view_transform.x) / scale;
        float world_y = (cursor_y - view_transform.y) / scale;
    }
};

struct GraphicsTest : Extension
{
    bool panel_toggle = false;
    float delta = 0;

    ConsoleWindow t1 =
        ConsoleWindow([](Input &input) { return input.IsHeld(Key::LControl) && input.IsPressed(Key::Num1); });

    ConsoleWindow t2 =
        ConsoleWindow([](Input &input) { return input.IsHeld(Key::LControl) && input.IsPressed(Key::Num2); });

    FocusManager focus_manager = FocusManager();

    CadController cad_controller;
    CadWorld world;

    GraphicsTest()
    {
        t1.SetSize(180, 320);
        t1.SetTitle("Console 1");

        t2.SetPosition(300, 300);
        t2.SetSize(300, 150);
        t2.SetTitle("Console 2");
        t2.GetRoot().SetLayoutStrategy(std::make_unique<GridLayoutStrategy>(3, 3));

        focus_manager.RegisterWindow(&t1);
        focus_manager.RegisterWindow(&t2);
    }

    void OnStart(StaticContext &context) override
    {
        context.fonts.LoadFont("default", "assets/font/dogica.bin", 8);
    }

    Camera camera;

    void OnUpdate(State state) override
    {
        state.graphics.Clear(Pixel(0x20, 0x20, 0x20));

        cad_controller.Update(state);
        world.Update(state, cad_controller.camera);

        // ------------------------------------------------------
        Font &default_font = state.static_context.fonts.GetFont("default");
        FontGraphics font_graphics(state.graphics, default_font);
        font_graphics.SetDisplayWidth(8);
        delta += state.chrono.GetDelta();

        Pallette pallette = Pallette();
        pallette.background = Color::BLACK;
        pallette.foreground = Color::WHITE;
        pallette.highlight = Color::RED;
        pallette.border = Color::WHITE;

        focus_manager.Update(state, font_graphics, pallette);

        if (cad_controller.current_mode != nullptr)
            font_graphics.DrawString(Color::WHITE, cad_controller.current_mode->GetName(), 0, 0);
    }
};

int main(void)
{
    std::shared_ptr<GraphicsTest> test = std::make_shared<GraphicsTest>();
    Engine engine(1300, 1300, 500, 500);
    engine.AddExtension(test);
    engine.Launch();
}