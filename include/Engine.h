#pragma once

#include <algorithm>
#include <chrono>
#include <climits>
#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <stack>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Engine;
struct Program;
struct StaticContext;
struct State;
class Viewport;

struct Pixel;
class Image;
class Graphics;
class TextRenderer;
struct Pallette;

struct Chronometer;
class Clock;
class Counter;
class PulseTimer;

struct OutputStream;
struct InputState;
class Input;
class TextType;
struct Debouncer;
struct DeboncedKey;

struct Vector2;
struct Matrix3;
struct Transform;

class Font;
class FontGraphics;
class FontManager;

class Camera;
class CameraController;

class Component;
class WindowManager;
class LayoutStrategy;
class Panel;
class Window;
using Shortcut = std::function<bool(Input& input)>;

class InteractionLock;
/* ------------------------------------------------------------------------------------------------------------------ */
/*                                                    Core Function                                                   */
/* ------------------------------------------------------------------------------------------------------------------ */
struct State {
    Chronometer& chrono;
    Input& input;
    Graphics& graphics;
    StaticContext& static_context;

    // Exports common engine components to extensions for use
    State(Chronometer& chrono, Input& input, Graphics& graphics, StaticContext& static_context)
        : chrono(chrono), input(input), graphics(graphics), static_context(static_context)
    {
    }
};

struct Program {
    virtual void OnStart(State& state){};
    virtual void OnUpdate(State state){};
    virtual void OnShutdown(){};
};

struct StaticContext {
    FontManager& fonts;
    OutputStream& output;
    WindowManager& window_manager;
    Pallette& pallette;

    StaticContext(FontManager& fonts, OutputStream& stream, WindowManager& windows, Pallette& pallette)
        : fonts(fonts), output(stream), window_manager(windows), pallette(pallette)
    {
    }
};
struct Pixel {
    uint8_t r, g, b, a = 0;

    Pixel() = default;
    Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    Pixel Darker() const;
    Pixel Lighter() const;
};

namespace Color {
// C64 Pallette of Colors
const Pixel BLACK(0, 0, 0);
const Pixel DARK_GRAY(64, 64, 64);
const Pixel GRAY(128, 128, 128);
const Pixel LIGHT_GRAY(192, 192, 192);
const Pixel WHITE(255, 255, 255);

const Pixel RED(136, 0, 0);
const Pixel CYAN(170, 255, 238);
const Pixel PURPLE(204, 68, 204);
const Pixel GREEN(0, 204, 85);
const Pixel BLUE(0, 0, 170);
const Pixel YELLOW(238, 238, 119);
const Pixel ORANGE(221, 136, 85);
const Pixel BROWN(102, 68, 0);

} // namespace Color

class Image {
    Pixel* m_data;
    unsigned m_width, m_height;

  public:
    Image(unsigned width, unsigned height);
    ~Image();

    uint8_t* GetData();
    unsigned GetWidth() const;
    unsigned GetHeight() const;

    Pixel GetPixel(unsigned x, unsigned y) const;
    Pixel SamplePixel(float x, float y) const;
    void SetPixel(unsigned x, unsigned y, Pixel pixel);
    void Clear(const Pixel& pixel = Color::BLACK);

    Graphics& GetGraphics();
};
class Graphics {
    std::shared_ptr<Image> m_image;
    std::stack<Transform> m_transform_stack;

  public:
    Graphics(std::shared_ptr<Image> image);
    ~Graphics() = default;

    unsigned GetWidth() const;
    unsigned GetHeight() const;
    Vector2 GetCenter() const;
    Vector2 GetDimensions() const;

    void Clear(Pixel color = Color::BLACK);

    // Does not apply transform
    void SetPixel(Pixel color, unsigned x, unsigned y);

    void DrawLine(Pixel color, float x0, float y0, float x1, float y1);
    void DrawRect(Pixel color, unsigned x, unsigned y, unsigned width, unsigned height);
    void DrawCircle(Pixel color, float x, float y, float radius);
    void DrawTriangle(Pixel color, float x0, float y0, float x1, float y1, float x2, float y2)
    {
        DrawLine(color, x0, y0, x1, y1);
        DrawLine(color, x1, y1, x2, y2);
        DrawLine(color, x2, y2, x0, y0);
    }
    void FillRect(Pixel color, unsigned x, unsigned y, unsigned width, unsigned height);
    void FillCircle(Pixel color, unsigned x, unsigned y, unsigned radius);

    // Todo: Move to dedicated image graphics class since rasters are tricky to do correctly
    void DrawImage(const Image& image, unsigned x, unsigned y, unsigned w, unsigned h);

    void PushTransform(const Transform& transform);
    Transform PopTransform();
    Transform GetTransform() const;
};

class TextRenderer {
    unsigned x, y, width, height;
    unsigned padding;
    unsigned draw_x = 0;
    unsigned draw_y = 0;
    void draw_glyph(Pixel color, FontGraphics& font_graphics, char c, unsigned draw_x, unsigned draw_y);

  public:
    TextRenderer(unsigned x, unsigned y, unsigned width, unsigned height, unsigned padding, unsigned scroll_offset)
        : x(x), y(y), width(width), height(height), padding(padding), draw_y(scroll_offset)
    {
    }

    void DrawText(Pixel color, std::string text, FontGraphics& font);
};

struct Pallette {
    Pixel foreground = Color::BLACK;
    Pixel background = Color::WHITE;
    Pixel border = Color::BLACK;
    Pixel highlight = Color::RED;
};
/* ------------------------------------------------------------------------------------------------------------------ */
/*                                                 Time and Automation                                                */
/* ------------------------------------------------------------------------------------------------------------------ */
struct Chronometer {
    virtual double GetTime() const = 0;
    virtual double GetDelta() const = 0;
};

class Clock : public Chronometer {
    double m_start_time;
    double m_last_time;

    double m_time;
    double m_delta;

  public:
    Clock();

    void Tick();

    double GetTime() const override;
    double GetDelta() const override;
};

class Counter {
    std::pair<float, float> range;
    float value;

  public:
    Counter(float start, float end) : range({start, end}), value(start) {}

    float Update(Chronometer& chrono)
    {
        value += chrono.GetDelta();
        if (value > range.second) {
            value = range.first;
        }
        return value;
    }

    float GetValue() const { return value; }

    void Reset() { value = range.first; }

    float GetStart() { return range.first; }

    float GetEnd() { return range.second; }
};

class PulseTimer {
    // Oscillates between two values of equal

    Counter counter;

  public:
    PulseTimer(float duration) : counter(0.0f, duration) {}

    bool Update(Chronometer& chrono)
    {
        counter.Update(chrono);
        float value = counter.GetValue();
        float half_way = counter.GetEnd() / 2.0f;
        return value > half_way;
    }

    void Reset() { counter.Reset(); }
};
/* ------------------------------------------------------------------------------------------------------------------ */
/*                                                    Input and IO                                                    */
/* ------------------------------------------------------------------------------------------------------------------ */
enum class Key : unsigned char {
    // clang-format off
  A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
  Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
  Escape, LControl, LShift, LAlt, LSystem, RControl, RShift, RAlt, RSystem,
  Menu, LBracket, RBracket, Semicolon, Comma, Period, Quote, Slash, Backslash,
  Tilde, Equal, Hyphen, Space, Enter, Backspace, Tab, PageUp, PageDown, End, Home,
  Insert, Delete, Add, Subtract, Multiply, Divide,
  Left, Right, Up, Down,
  Kp0, Kp1, Kp2, Kp3, Kp4, Kp5, Kp6, Kp7, Kp8, Kp9,
  KpDivide, KpMultiply, KpSubtract, KpAdd, KpDecimal, KpEnter,
  Unknown
    // clang-format on
};

enum class MouseButton : unsigned char {
    LEFT = 0,
    RIGHT = 1,
    MIDDLE = 2
};

struct OutputStream {
    std::vector<std::string> lines;

    OutputStream() = default;

    void Flush() {}

    void Println(std::string text) { lines.push_back(text); }

    std::vector<std::string> GetLines() { return lines; }
};

struct InputState {
    std::vector<char> char_buffer;

    float last_mouse_x, last_mouse_y;
    float current_mouse_x, current_mouse_y;
    float scroll_delta;

    bool previous_mouse[3];
    bool current_mouse[3];

    bool previous_keys[256];
    bool current_keys[256];

    InputState();
    void Update();
    void Clear();
};

// Given to Client code to allow for simple querying of input state
class Input {
    InputState& m_input_state;

  public:
    Input(InputState& input_state) : m_input_state(input_state) {}

    bool IsPressed(Key key) const;
    bool IsPressed(MouseButton button) const;

    bool IsReleased(Key key) const;
    bool IsReleased(MouseButton button) const;

    bool IsHeld(Key key) const;
    bool IsHeld(MouseButton button) const;

    bool IsDown(Key key) const;
    bool IsDown(MouseButton button) const;

    float GetMouseX() const;
    float GetMouseY() const;
    Vector2 GetMousePosition() const;

    float GetMouseDeltaX() const;
    float GetMouseDeltaY() const;

    float GetScrollDeltaX() const;
    float GetScrollDeltaY() const;

    std::string GetText() const;
};

struct Debouncer {
    enum State {
        Idle,
        Pressed,
        Held
    };

    State state = Idle;
    float current_timer = 0.0f;
    Key matching_key;

  public:
    Debouncer(Key matching_key) : matching_key(matching_key) {}

    void Update(Input& input, Chronometer& chrono)
    {
        switch (state) {
        case Idle:
            if (input.IsPressed(matching_key)) {
                state = Pressed;
                current_timer = 0.0f;
            }
            break;
        case Pressed:
            if (input.IsHeld(matching_key)) {
                current_timer += chrono.GetDelta();
                if (current_timer > 0.35f) {
                    state = Held;
                }
            } else {
                state = Idle;
            }
            break;
        case Held:
            if (input.IsReleased(matching_key)) {
                state = Idle;
            }
            break;
        default: break;
        }
    }

    bool IsHeld() const { return state == Held; }
};

struct DebouncedKey {
    Debouncer debouncer;
    PulseTimer wait_timer;

  public:
    DebouncedKey(Key key, float time) : debouncer(key), wait_timer(time) {}

    bool Update(State state)
    {
        auto& input = state.input;
        auto& chrono = state.chrono;

        debouncer.Update(input, chrono);
        bool is_up_time = wait_timer.Update(chrono);
        if (debouncer.IsHeld() && is_up_time) {
            wait_timer.Reset();
            return true;
        }

        return input.IsPressed(debouncer.matching_key);
    }
};

class TextType {
    std::string m_text_buffer;

    Debouncer backspace_debouncer = Debouncer(Key::Backspace);
    PulseTimer backspace_timer = PulseTimer(0.325f);

    void do_backspace(Input& input, Chronometer& chrono)
    {
        if (input.IsPressed(Key::Backspace) && m_text_buffer.size() > 0) {
            m_text_buffer.pop_back();
        }

        backspace_debouncer.Update(input, chrono);
        bool is_backspace_time = backspace_timer.Update(chrono);
        if (backspace_debouncer.IsHeld() && is_backspace_time && m_text_buffer.size() > 0) {
            m_text_buffer.pop_back();
            backspace_timer.Reset();
        }
    }

  public:
    void Update(Input& input, Chronometer& chrono)
    {
        m_text_buffer += input.GetText();

        do_backspace(input, chrono);
    }

    std::string GetText() { return m_text_buffer; }

    void Clear() { m_text_buffer.clear(); }
};
/* ------------------------------------------------------------------------------------------------------------------ */
/*                                                  Math and Geometry                                                 */
/* ------------------------------------------------------------------------------------------------------------------ */
struct Vector2 {
    float x, y = 0;

    Vector2() = default;
    Vector2(float x, float y);

    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(const Vector2& other) const;
    Vector2 operator/(const Vector2& other) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;

    Vector2 Rotate(float angle) const
    {
        return Vector2(x * cos(angle) - y * sin(angle), x * sin(angle) + y * cos(angle));
    }
    float Length() const { return sqrt(x * x + y * y); }
    Vector2 ClosestPointOnLine(Vector2 line_start, Vector2 line_end) const
    {
        Vector2 line = line_end - line_start;
        float line_length = line.Length();
        Vector2 line_direction = line / line_length;
        Vector2 point = *this - line_start;
        float dot = point.x * line_direction.x + point.y * line_direction.y;
        dot = std::max(0.0f, std::min(dot, line_length));
        return line_start + line_direction * dot;
    }

    static bool IsInBounds(Vector2 point, Vector2 position, Vector2 size);

    static Vector2 FromAngle(float angle) { return Vector2(cos(angle), sin(angle)); }
};

struct Matrix3 {
    float data[9];

    Matrix3();

    Matrix3 operator*(const Matrix3& other) const;
    Vector2 operator*(const Vector2& other) const;

    static Matrix3 Identity();
    static Matrix3 Translation(float x, float y);
    static Matrix3 Rotation(float angle);
    static Matrix3 Scale(float x, float y);
};

struct Transform {
    float x, y, scale, rotation;

    Transform(float x, float y, float scale, float rotation);
    Transform();

    static Transform Identity();
    Transform& Translate(float x, float y);
    Transform& Scale(float scale);
    Transform& Rotate(float rotation);

    Vector2 Apply(Vector2 point) const;

    float GetX() const { return x; }

    float GetY() const { return y; }

    float GetScale() const { return scale; }

    float GetRotation() const { return rotation; }
};
/* ------------------------------------------------------------------------------------------------------------------ */
/*                                                        Font                                                        */
/* ------------------------------------------------------------------------------------------------------------------ */
class Font {
    unsigned m_width, m_height;
    unsigned m_glyph_width;
    std::vector<Pixel> m_pixels;

  public:
    Font(std::vector<Pixel> pixels, unsigned glyph_width);

    unsigned GetGlyphWidth() const;
    bool GetGlyph(unsigned char character, unsigned x, unsigned y) const;

    static Font LoadFromBin(const char* filename, unsigned glyph_width = 16);
};

class FontGraphics {
    unsigned m_display_width = 8;
    Font& font;
    Graphics& graphics;

  public:
    FontGraphics(Graphics& graphics, Font& font);

    unsigned GetDisplayWidth() const { return m_display_width; }
    void SetDisplayWidth(unsigned width) { m_display_width = width; }
    void DrawGlyph(Pixel color, unsigned char character, unsigned x, unsigned y);
    void DrawString(Pixel color, std::string str, unsigned x, unsigned y);
    Vector2 MeasureString(std::string str) const;
};

class FontManager {
    std::unordered_map<std::string, std::unique_ptr<Font>> m_fonts;

  public:
    FontManager() = default;

    void LoadFont(const std::string& name, const char* filename, unsigned glyph_width = 16)
    {
        // TODO: Add error handling
        m_fonts[name] = std::make_unique<Font>(Font::LoadFromBin(filename, glyph_width));
    };

    Font& GetFont(const std::string& name)
    {
        // TODO: Add error handling
        return *m_fonts[name];
    }
};

class WindowManager {
    std::vector<Window*> windows;    // all windows that are registered
    Window* active_window = nullptr; // the window that has focus
    bool has_given_focus = false;    // we will only fulfill one focus request per frame

  public:
    void Update(State state);

    void RegisterWindow(Window* window);
    void RequestFocus(Window* window);
    bool HasFocus(Window* window);
};

class Viewport {
    InputState m_input_state;
    std::unique_ptr<Image> framebuffer;
    unsigned m_width, m_height;
    void* m_data;

  public:
    Viewport(unsigned width, unsigned height, unsigned buffer_width, unsigned buffer_height) throw();
    ~Viewport();

    void Flush();
    void UpdateFramebuffer();
    Input UpdateInput();
    Graphics GetGraphics();

    bool IsOpen() const;
    unsigned GetWidth() const;
    unsigned GetHeight() const;
    void SetTitle(const std::string& title);
};

class Engine {
    std::unique_ptr<Viewport> viewport;
    FontManager m_font_manager;
    OutputStream m_output_stream;
    WindowManager m_window_manager;
    Pallette m_pallette;

  public:
    Engine(unsigned view_width, unsigned view_height, unsigned buffer_width, unsigned buffer_height);
    ~Engine();
    void Launch(Program& program);
};
// =====================================================================================================================
struct Camera {
    float pan_x = 0;
    float pan_y = 0;
    float scale = 1;

    Camera() = default;

    Transform GetTransform() const
    {
        Transform transform;
        transform.Translate(pan_x, pan_y);
        transform.Scale(scale);
        return transform;
    }

    Vector2 ScreenToWorld(Vector2 screen_pos) const
    {
        Transform view_transform = GetTransform();
        float world_x = (screen_pos.x - view_transform.x) / view_transform.scale;
        float world_y = (screen_pos.y - view_transform.y) / view_transform.scale;
        return Vector2(world_x, world_y);
    }

    Vector2 WorldToScreen(Vector2 world_pos) const
    {
        Transform view_transform = GetTransform();
        return view_transform.Apply(world_pos);
    }
};
// =====================================================================================================================
namespace Shortcuts {
using Shortcut = std::function<bool(Input&)>;
const Shortcut ctrl_n1 = [](Input& input) { return input.IsHeld(Key::LControl) && input.IsPressed(Key::Num1); };
const Shortcut ctrl_n2 = [](Input& input) { return input.IsHeld(Key::LControl) && input.IsPressed(Key::Num2); };
} // namespace Shortcuts

enum class Alignment {
    START,
    MIDDLE,
    END
};

struct Component {
    Vector2 m_position;
    Vector2 m_size;

  public:
    Component(unsigned width, unsigned height) : m_position(0, 0), m_size(width, height) {}

    Vector2 GetPosition() const;
    Vector2 GetSize() const;
    void SetPosition(unsigned x, unsigned y);
    void SetSize(unsigned width, unsigned height);
};

class Interactor;
class InteractionLock;

class InteractionLock {
    bool is_locked = false;
    std::vector<Interactor*> interactors;

  public:
    bool IsLocked() const;
    void Lock(Interactor& interactor);
    void Unlock(Interactor& interactor);
    void Clear();
    void Notify();

    void Subscribe(Interactor* interactor);
    void Unsubscribe(Interactor* interactor);
};

class Interactor {
    friend class InteractionLock;
    InteractionLock& lock;
    bool has_lock = false;

  public:
    Interactor(InteractionLock& lock); // subscribe to lock
    ~Interactor();                     // unsubscribe from lock

    bool HasLock() const;
    bool TryLock();
    void ForceLock();
    void ReleaseLock();
    void OnNotify();
};

class Window : public Component {
    class DragBar : public Component {
        bool is_being_dragged = false;
        bool has_lock = false;

      public:
        DragBar(unsigned width, unsigned height) : Component(width, height) {}

        void Update(State state, Window& parent, std::string title);
    };

    std::string m_title = "";
    DragBar m_drag_bar;
    std::unique_ptr<Panel> m_root_panel;

    Shortcut m_toggle_shortcut;
    bool m_is_open = false;

    Interactor static_interactor;

  public:
    Window(InteractionLock& lock, Shortcut toggle_shortcut, unsigned x, unsigned y, unsigned w, unsigned h)
        : m_toggle_shortcut(toggle_shortcut), m_drag_bar(w, 10), static_interactor(lock), Component(w, h)
    {
        m_root_panel = std::make_unique<Panel>(w, h);
    }

    void Update(State state);
    virtual void OnInput(State state) = 0;
    virtual void OnUpdate(State state) = 0;
    virtual void OnRender(State state) = 0;

    Panel& GetRoot() const { return *m_root_panel; }

    void SetTitle(std::string title) { this->m_title = title; }
};

struct ConsoleWindow : public Window {
    TextType text_type;
    PulseTimer cursor_timer = PulseTimer(1.0f);
    Debouncer scroll_up_debouncer = Debouncer(Key::Up);
    Debouncer scroll_down_debouncer = Debouncer(Key::Down);

    char cursor_character = '_';
    unsigned padding = 5;

    unsigned scroll_pixel_offset = 0;

    ConsoleWindow(InteractionLock& lock, Shortcut toggle_shortcut) : Window(lock, toggle_shortcut, 25, 25, 400, 400) {}

    void OnInput(State state) override;
    void OnUpdate(State state) override;
    void OnRender(State state) override;
};

class Panel : public Component {
    std::vector<std::shared_ptr<Panel>> m_children;
    std::unique_ptr<LayoutStrategy> m_layout_strategy;

  public:
    Panel(unsigned width, unsigned height);

    void Input(State& state);
    void Update(State& state);
    void Render(State& state);

    virtual void OnInput(State& state) {}
    virtual void OnRender(State& state) {}
    virtual void OnUpdate(State& state) {}

    void AddChild(std::shared_ptr<Panel> child);
    void SetLayoutStrategy(std::unique_ptr<LayoutStrategy> layout_strategy)
    {
        m_layout_strategy = std::move(layout_strategy);
    }
};

class LayoutStrategy {
    Alignment alignment = Alignment::START;

  public:
    virtual void Layout(Panel& parent, Panel& child) = 0;
    virtual void Reset(unsigned x, unsigned y) = 0;

    Alignment GetAlignment();
    void SetAlignment(Alignment alignment);
};

class VerticalLayout : public LayoutStrategy {
    unsigned layout_x;
    unsigned layout_y;

  public:
    void Layout(Panel& parent, Panel& child) override;
    void Reset(unsigned x, unsigned y) override;
};

class HorizontalLayout : public LayoutStrategy {
    unsigned layout_x;
    unsigned layout_y;

  public:
    void Layout(Panel& parent, Panel& child) override;
    void Reset(unsigned x, unsigned y) override;
};

struct TextArea : public Panel {
    // TODO: Add focus mechanic.  I think an event stream with consumable events is the way to go.
    TextType text_type;
    PulseTimer cursor_timer = PulseTimer(1.0f);
    Debouncer scroll_up_debouncer = Debouncer(Key::Up);
    Debouncer scroll_down_debouncer = Debouncer(Key::Down);

    Pallette pallette;

    char cursor_character = '_';
    unsigned padding = 5;
    unsigned scroll_pixel_offset = 0;

    TextArea(unsigned width, unsigned height) : Panel(width, height) {}

    void OnUpdate(State& state) override
    {
        Font& font = state.static_context.fonts.GetFont("default");
        FontGraphics font_graphics(state.graphics, font);
        font_graphics.SetDisplayWidth(8);

        text_type.Update(state.input, state.chrono);
        std::string text = text_type.GetText();

        scroll_up_debouncer.Update(state.input, state.chrono);
        if (state.input.IsPressed(Key::Up) || scroll_up_debouncer.IsHeld()) {
            scroll_pixel_offset -= 1;
        }

        scroll_down_debouncer.Update(state.input, state.chrono);
        if (state.input.IsPressed(Key::Down) || scroll_down_debouncer.IsHeld()) {
            scroll_pixel_offset += 1;
        }

        if (scroll_pixel_offset < 0) {
            scroll_pixel_offset = 0;
        }

        unsigned x = GetPosition().x;
        unsigned y = GetPosition().y;
        unsigned width = GetSize().x;
        unsigned height = GetSize().y;

        // Draw the background
        // state.graphics.FillRect(pallette.background, x, y, width, height);
        state.graphics.DrawRect(pallette.border, x, y, width, height);

        TextRenderer text_render(
            x + padding, y + padding, width - padding, height - padding, padding, scroll_pixel_offset);
        text_render.DrawText(pallette.foreground, text_type.GetText(), font_graphics);

        // Update the cursor and draw it to the screen
        cursor_character = cursor_character == ' ' ? '_' : ' ';

        std::string cursor_string = std::string(1, this->cursor_character);
        text_render.DrawText(pallette.foreground, cursor_string, font_graphics);
    }
};
