#pragma once

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
struct Input;
class Viewport;
struct Extension;
struct State;
class Engine;

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
const Pixel BLACK(0, 0, 0);
const Pixel GRAY(128, 128, 128);
const Pixel WHITE(255, 255, 255);
const Pixel RED(255, 0, 0);
const Pixel GREEN(0, 255, 0);
const Pixel BLUE(0, 0, 255);
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

class Graphics
{
    std::shared_ptr<Image> m_image;
    Pixel m_color;

  public:
    Graphics(std::shared_ptr<Image> image);
    ~Graphics() = default;

    void SetColor(const Pixel &pixel);
    void Clear(const Pixel &pixel = Color::BLACK);

    void DrawPixel(unsigned x, unsigned y);

    void DrawLine(unsigned x0, unsigned y0, unsigned x1, unsigned y1);
    void DrawRect(unsigned x, unsigned y, unsigned width, unsigned height);
    void DrawCircle(unsigned x, unsigned y, unsigned radius);
    void DrawEllipse(unsigned x, unsigned y, unsigned radius_x, unsigned radius_y);

    void FillRect(unsigned x, unsigned y, unsigned width, unsigned height);
    void FillCircle(unsigned x, unsigned y, unsigned radius);

    void DrawImage(const Image &image, unsigned x, unsigned y, unsigned w, unsigned h);

    unsigned GetWidth() const;
    unsigned GetHeight() const;
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

    // @returns (global time, delta time)
    std::pair<float, float> Tick();

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

struct State
{
    // Exports common engine components to extensions for use
    State(Chronometer &chrono, Input &input, Graphics &graphics) : chrono(chrono), input(input), graphics(graphics)
    {
    }

    Chronometer &chrono;
    Input &input;
    Graphics &graphics;
};

class Input
{
    friend class Viewport;
    float last_mouse_x, last_mouse_y = 0;

  public:
    Input() = default;

    bool keys[256];
    bool mouse_left, mouse_right = false;
    float mouse_x, mouse_y = 0;
};

class Viewport
{
    std::unique_ptr<Image> framebuffer;
    unsigned m_width, m_height;
    void *m_data;
    Input input;

  public:
    Viewport(unsigned width, unsigned height, unsigned viewarea_width, unsigned viewarea_height) throw();
    ~Viewport();

    void Flush();
    void UpdateFramebuffer();
    Input &UpdateInput();
    Graphics &GetGraphics();

    bool IsOpen() const;
    unsigned GetWidth() const;
    unsigned GetHeight() const;
    void SetTitle(const std::string &title);
};

struct Extension
{
    virtual void OnStart(){};
    virtual void OnUpdate(State state){};
    virtual void OnShutdown(){};
};

class Engine
{
    std::unique_ptr<Viewport> viewport;
    std::vector<std::shared_ptr<Extension>> m_extensions;

    // Debug Stats
    double accumulated_delta_time = 0;
    unsigned accumlated_updates_count = 0;
    void updateDebugStats(double delta_time);

  public:
    Engine(unsigned view_width, unsigned view_height, unsigned buffer_width, unsigned buffer_height);
    void AddExtension(std::shared_ptr<Extension> extension);
    void Launch();
    ~Engine();
};

// =====================================================================================================================
