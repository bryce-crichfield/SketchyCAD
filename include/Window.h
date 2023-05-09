#pragma once

#include "Engine.h"

// Oscillates between two values of equal
struct PulseTimer
{
    Counter counter;

    PulseTimer(float duration) : counter(0.0f, duration)
    {
    }

    bool Update(Chronometer &chrono)
    {
        counter.Update(chrono);
        float value = counter.GetValue();
        float half_way = counter.range.second / 2.0f;
        return value > half_way;
    }

    void Reset()
    {
        counter.Reset();
    }
};

class TextType
{
    std::string m_text_buffer;

    Debouncer backspace_debouncer = Debouncer(Key::Backspace);
    PulseTimer backspace_timer = PulseTimer(0.325f);

    void do_backspace(Input &input, Chronometer &chrono)
    {
        if (input.IsPressed(Key::Backspace) && m_text_buffer.size() > 0)
        {
            m_text_buffer.pop_back();
        }

        backspace_debouncer.Update(input, chrono);
        bool is_backspace_time = backspace_timer.Update(chrono);
        if (backspace_debouncer.IsHeld() && is_backspace_time && m_text_buffer.size() > 0)
        {
            m_text_buffer.pop_back();
            backspace_timer.Reset();
        }
    }

  public:
    void Update(Input &input, Chronometer &chrono)
    {
        m_text_buffer += input.GetText();

        do_backspace(input, chrono);
    }

    std::string GetText()
    {
        return m_text_buffer;
    }

    void Clear()
    {
        m_text_buffer.clear();
    }
};

class TextRenderer
{
    unsigned x, y, width, height;
    unsigned padding;
    unsigned draw_x = 0;
    unsigned draw_y = 0;
    void draw_glyph(Pixel color, FontGraphics &font_graphics, char c, unsigned draw_x, unsigned draw_y);
  public:
    TextRenderer(unsigned x, unsigned y, unsigned width, unsigned height, unsigned padding, unsigned scroll_offset)
        : x(x), y(y), width(width), height(height), padding(padding), draw_y(scroll_offset)
    {
    }

    void DrawText(Pixel color, std::string text, FontGraphics &font);
};
// =====================================================================================================================

struct Pallette;
struct Component;
struct FocusManager;
struct LayoutStrategy;
struct Panel;
struct Window;
using Shortcut = std::function<bool(Input &input)>;


struct Pallette {
    Pixel foreground = Color::BLACK;
    Pixel background = Color::WHITE;
    Pixel border = Color::BLACK;
    Pixel highlight = Color::RED;
};

enum class Alignment {
    START, MIDDLE, END
};

struct Component
{
    Vector2 m_position;
    Vector2 m_size;

  public:
    Component(unsigned x, unsigned y, unsigned width, unsigned height) : m_position(x, y), m_size(width, height)
    {
    }

    Vector2 GetPosition() const;
    Vector2 GetSize() const;
    void SetPosition(unsigned x, unsigned y);
    void SetSize(unsigned width, unsigned height);
};

class LayoutStrategy {
    Alignment alignment = Alignment::START;
public:
    virtual void Layout(Panel &parent, Panel &child) = 0;
    virtual void Reset(unsigned x, unsigned y) = 0;

    Alignment GetAlignment();
    void SetAlignment(Alignment alignment);
};

class VerticalLayout : public LayoutStrategy {
    unsigned layout_x;
    unsigned layout_y;
public:
    void Layout(Panel &parent, Panel &child) override;
    void Reset(unsigned x, unsigned y) override;
};

class Panel : public Component {
    std::vector<std::unique_ptr<Panel>> m_children;
    std::unique_ptr<LayoutStrategy> m_layout_strategy;

  public:
    Panel(unsigned x, unsigned y, unsigned width, unsigned height) : Component(x, y, width, height) {
        m_layout_strategy = std::make_unique<VerticalLayout>();
    }

    void Update(State &state);
    virtual void OnUpdate(State &state) = 0;

    void AddChild(std::unique_ptr<Panel> child);
};

struct TestPanel : public Panel {
    TestPanel(unsigned x, unsigned y, unsigned width, unsigned height) : Panel(x, y, width, height) {}

    void OnUpdate(State &state) override {
        state.graphics.DrawRect(Color::RED, GetPosition().x, GetPosition().y, GetSize().x, GetSize().y);
    }
};

class DragBar : public Component
{
    bool is_being_dragged = false;
    bool has_lock = false;

  public:
    DragBar(unsigned x, unsigned y, unsigned width, unsigned height) : Component(x, y, width, height)
    {
    }

    void Update(FocusManager& manager, State &state, Window &parent, Pallette pallette, FontGraphics& font_graphics, std::string title);
};

struct Window : public Component
{
    std::string title = "";
    DragBar title_bar;
    std::unique_ptr<Panel> root_panel;

    Shortcut toggle_shortcut;
    bool is_open = false;

    Window(Shortcut toggle_shortcut, unsigned x, unsigned y, unsigned w, unsigned h)
        : toggle_shortcut(toggle_shortcut), Component(x, y, w, h), title_bar(x, y, w, 20)
    {
        root_panel = std::make_unique<TestPanel>(x, y, w, h);
        root_panel->AddChild(std::make_unique<TestPanel>(x, y, w, 50));
        root_panel->AddChild(std::make_unique<TestPanel>(x, y, w, 50));
        root_panel->AddChild(std::make_unique<TestPanel>(x, y, w, 50));
    }

    void Update(FocusManager& manager, State &state, FontGraphics &font_graphics, Pallette pallette);

    virtual void OnUpdate(FocusManager& manager, State &state, FontGraphics &font_graphics, Pallette pallette) = 0;

    void SetTitle(std::string title)
    {
        this->title = title;
    }
};

struct ConsoleWindow : public Window
{
    TextType text_type;
    PulseTimer cursor_timer = PulseTimer(1.0f);
    Debouncer scroll_up_debouncer = Debouncer(Key::Up);
    Debouncer scroll_down_debouncer = Debouncer(Key::Down);

    char cursor_character = '_';
    unsigned padding = 5;

    unsigned scroll_pixel_offset = 0;

    ConsoleWindow(Shortcut toggle_shortcut) : Window(toggle_shortcut, 25, 25, 400, 400)
    {
    }

    void OnUpdate(FocusManager& manager, State &state, FontGraphics &font_graphics, Pallette pallette) override;
};

class FocusManager {
    std::vector<Window *> windows;          // all windows that are registered
    Window* active_window = nullptr;        // the window that has focus
    bool has_given_focus = false;           // we will only fulfill one focus request per frame
public:
    void Update(State &state, FontGraphics &font_graphics, Pallette pallette);
    void RegisterWindow(Window *window);
    void RequestFocus(Window *window);
    bool HasFocus(Window *window);
};