#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <queue>
#include <stack>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct Pixel;
class Image;
class Graphics;
struct Chronometer;
class Clock;

struct InputState;
class Input;
class TextEntry;
struct Extension;
struct State;
class Engine;

class Font;
class FontGraphics;
class FontManager;

struct Vector2;
struct Transform;

struct Pixel
{
    uint8_t r, g, b, a = 0;

    Pixel() = default;
    Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    Pixel Darker() const;
    Pixel Lighter() const;
};

namespace Color
{
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

class Image
{
    Pixel *m_data;
    unsigned m_width, m_height;

  public:
    Image(unsigned width, unsigned height);
    ~Image();

    uint8_t *GetData();
    unsigned GetWidth() const;
    unsigned GetHeight() const;

    Pixel GetPixel(unsigned x, unsigned y) const;
    Pixel SamplePixel(float x, float y) const;
    void SetPixel(unsigned x, unsigned y, Pixel pixel);
    void Clear(const Pixel &pixel = Color::BLACK);

    Graphics &GetGraphics();
};

struct Transform
{
    float x, y, scale, rotation;

    Transform(float x, float y, float scale, float rotation);
    Transform();

    static Transform Identity();
    Transform &Translate(float x, float y);
    Transform &Scale(float scale);
    Transform &Rotate(float rotation);

    Vector2 Apply(Vector2 point) const;

    float GetX() const
    {
        return x;
    }

    float GetY() const
    {
        return y;
    }

    float GetScale() const
    {
        return scale;
    }

    float GetRotation() const
    {
        return rotation;
    }

    Transform GetInverse() const
    {
        Transform inverse;
        inverse.Translate(x * -1, y * -1);
        // inverse.Scale(1 / scale);
        return inverse;
    }
};

class Graphics
{
    std::shared_ptr<Image> m_image;
    std::stack<Transform> m_transform_stack;

  public:
    Graphics(std::shared_ptr<Image> image);
    ~Graphics() = default;

    unsigned GetWidth() const;
    unsigned GetHeight() const;

    void Clear(Pixel color = Color::BLACK);

    // Does not apply transform
    void SetPixel(Pixel color, unsigned x, unsigned y);

    void DrawLine(Pixel color, float x0, float y0, float x1, float y1);
    void DrawRect(Pixel color, unsigned x, unsigned y, unsigned width, unsigned height);
    void DrawCircle(Pixel color, float x, float y, float radius);
    void FillRect(Pixel color, unsigned x, unsigned y, unsigned width, unsigned height);
    void FillCircle(Pixel color, unsigned x, unsigned y, unsigned radius);

    // Todo: Move to dedicated image graphics class since rasters are tricky to do correctly
    void DrawImage(const Image &image, unsigned x, unsigned y, unsigned w, unsigned h);

    void PushTransform(const Transform &transform);
    Transform PopTransform();
    Transform GetTransform() const;
};

struct Chronometer
{
    virtual double GetTime() const = 0;
    virtual double GetDelta() const = 0;
};

class Clock : public Chronometer
{
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

enum class Key : unsigned char
{
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

struct Dimension
{
    unsigned width, height;

    Dimension(unsigned width, unsigned height) : width(width), height(height)
    {
    }
};

struct StaticContext
{
    FontManager &fonts;

    StaticContext(FontManager &fonts) : fonts(fonts)
    {
    }
};

struct State
{
    Chronometer &chrono;
    Input &input;
    Graphics &graphics;
    StaticContext &static_context;

    // Exports common engine components to extensions for use
    State(Chronometer &chrono, Input &input, Graphics &graphics, StaticContext &static_context)
        : chrono(chrono), input(input), graphics(graphics), static_context(static_context)
    {
    }
};

enum class MouseButton : unsigned char
{
    LEFT = 0,
    RIGHT = 1,
    MIDDLE = 2
};

struct InputState
{
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
class Input
{
    InputState &m_input_state;

  public:
    Input(InputState &input_state) : m_input_state(input_state)
    {
    }

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

    float GetMouseDeltaX() const;
    float GetMouseDeltaY() const;

    float GetScrollDeltaX() const;
    float GetScrollDeltaY() const;

    std::string GetText() const;
};

struct Counter
{
    std::pair<float, float> range;
    float value;

    Counter(float start, float end) : range({start, end}), value(start)
    {
    }

    float Update(Chronometer &chrono)
    {
        value += chrono.GetDelta();
        if (value > range.second)
        {
            value = range.first;
        }
        return value;
    }

    float GetValue() const
    {
        return value;
    }

    void Reset()
    {
        value = range.first;
    }
};

struct Debouncer
{
    enum State
    {
        Idle,
        Pressed,
        Held
    };

    State state = Idle;
    float current_timer = 0.0f;
    Key matching_key;

  public:
    Debouncer(Key matching_key) : matching_key(matching_key)
    {
    }

    void Update(Input &input, Chronometer &chrono)
    {
        switch (state)
        {
        case Idle:
            if (input.IsPressed(matching_key))
            {
                state = Pressed;
                current_timer = 0.0f;
            }
            break;
        case Pressed:
            if (input.IsHeld(matching_key))
            {
                current_timer += chrono.GetDelta();
                if (current_timer > 0.35f)
                {
                    state = Held;
                }
            }
            else
            {
                state = Idle;
            }
            break;
        case Held:
            if (input.IsReleased(matching_key))
            {
                state = Idle;
            }
            break;
        default:
            break;
        }
    }

    bool IsHeld() const
    {
        return state == Held;
    }
};

// TODO: SRP violation, acts as buffer and behavior (per backspace/cursor suggestion)
class InputText
{
    Debouncer m_debouncer;
    std::string m_text;

  public:
    InputText() : m_debouncer(Key::Backspace)
    {
    }

    void Update(Input &input, Chronometer &chrono)
    {
        m_text += input.GetText();
        m_debouncer.Update(input, chrono);
        if (m_debouncer.IsHeld())
        {
            m_text.pop_back();
        }
    }

    std::string GetText() const
    {
        return m_text;
    }

    void Clear()
    {
        m_text.clear();
    }
};

class Viewport
{
    InputState m_input_state;
    std::unique_ptr<Image> framebuffer;
    unsigned m_width, m_height;
    void *m_data;

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
    void SetTitle(const std::string &title);
};

struct Extension
{
    virtual void OnStart(StaticContext &static_context){};
    virtual void OnUpdate(State state){};
    virtual void OnShutdown(){};
};

class Engine
{
    std::unique_ptr<Viewport> viewport;
    std::vector<std::shared_ptr<Extension>> m_extensions;
    std::unique_ptr<FontManager> m_fonts;

    // Debug Stats
    double accumulated_delta_time = 0;
    unsigned accumlated_updates_count = 0;
    void updateDebugStats(double delta_time);

  public:
    Engine(unsigned view_width, unsigned view_height, unsigned buffer_width, unsigned buffer_height);
    ~Engine();
    void AddExtension(std::shared_ptr<Extension> extension);
    void Launch();
};
// =====================================================================================================================
struct Vector2
{
    float x, y = 0;

    Vector2() = default;
    Vector2(float x, float y);

    Vector2 operator+(const Vector2 &other) const;
    Vector2 operator-(const Vector2 &other) const;
    Vector2 operator*(const Vector2 &other) const;
    Vector2 operator/(const Vector2 &other) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;
    Vector2 Rotate(float angle) const
    {
        return Vector2(x * cos(angle) - y * sin(angle), x * sin(angle) + y * cos(angle));
    }
    static bool IsInBounds(Vector2 point, Vector2 position, Vector2 size);
    float Length() const {
        return sqrt(x * x + y * y);
    }
};

struct Matrix3
{
    float data[9];

    Matrix3();

    Matrix3 operator*(const Matrix3 &other) const;
    Vector2 operator*(const Vector2 &other) const;

    static Matrix3 Identity();
    static Matrix3 Translation(float x, float y);
    static Matrix3 Rotation(float angle);
    static Matrix3 Scale(float x, float y);
};
// =====================================================================================================================
class Font
{
    unsigned m_width, m_height;
    unsigned m_glyph_width;
    std::vector<Pixel> m_pixels;

  public:
    Font(std::vector<Pixel> pixels, unsigned glyph_width);

    unsigned GetGlyphWidth() const;
    bool GetGlyph(unsigned char character, unsigned x, unsigned y) const;

    static Font LoadFromBin(const char *filename, unsigned glyph_width = 16);
};

class FontGraphics
{
    unsigned m_display_width = 8;
    Font &font;
    Graphics &graphics;

  public:
    FontGraphics(Graphics &graphics, Font &font);

    unsigned GetDisplayWidth() const
    {
        return m_display_width;
    }
    void SetDisplayWidth(unsigned width)
    {
        m_display_width = width;
    }
    void DrawGlyph(Pixel color, unsigned char character, unsigned x, unsigned y);
    void DrawString(Pixel color, std::string str, unsigned x, unsigned y);
    Vector2 MeasureString(std::string str) const;
};

class FontManager
{
    std::unordered_map<std::string, std::unique_ptr<Font>> m_fonts;

  public:
    FontManager() = default;

    void LoadFont(const std::string &name, const char *filename, unsigned glyph_width = 16)
    {
        // TODO: Add error handling
        m_fonts[name] = std::make_unique<Font>(Font::LoadFromBin(filename, glyph_width));
    };

    Font &GetFont(const std::string &name)
    {
        // TODO: Add error handling
        return *m_fonts[name];
    }
};

// =====================================================================================================================
