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

struct Pallette
{
    Pixel foreground = Color::BLACK;
    Pixel background = Color::WHITE;
    Pixel border = Color::BLACK;
    Pixel highlight = Color::RED;
};

enum class Alignment
{
    START,
    MIDDLE,
    END
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

class LayoutStrategy
{
    Alignment alignment = Alignment::START;

  public:
    virtual void Layout(Panel &parent, Panel &child) = 0;
    virtual void Reset(unsigned x, unsigned y) = 0;

    Alignment GetAlignment();
    void SetAlignment(Alignment alignment);
};

class VerticalLayout : public LayoutStrategy
{
    unsigned layout_x;
    unsigned layout_y;

  public:
    void Layout(Panel &parent, Panel &child) override;
    void Reset(unsigned x, unsigned y) override;
};

class HorizontalLayout : public LayoutStrategy
{
    unsigned layout_x;
    unsigned layout_y;

  public:
    void Layout(Panel &parent, Panel &child) override;
    void Reset(unsigned x, unsigned y) override;
};

class GridLayoutStrategy : public LayoutStrategy
{
    unsigned layout_x;
    unsigned layout_y;
    unsigned index_x;
    unsigned index_y;
    unsigned grid_width;
    unsigned grid_height;

    public:
    GridLayoutStrategy(unsigned grid_width, unsigned grid_height) : grid_width(grid_width), grid_height(grid_height)
    {
    }

    void Layout(Panel &parent, Panel &child) override;
    void Reset(unsigned x, unsigned y) override;
};

struct Border
{
    virtual void Draw(Panel &panel, Graphics &graphics) = 0;
};

struct DownBorder : public Border
{
    void Draw(Panel &panel, Graphics &graphics) override
    {
        // TODO: Implement
    }
};

class Panel : public Component
{
    std::vector<std::unique_ptr<Panel>> m_children;
    std::unique_ptr<LayoutStrategy> m_layout_strategy;
    std::unique_ptr<Border> m_border;

  public:
    Panel(unsigned width, unsigned height) : Component(0, 0, width, height)
    {
        m_layout_strategy = std::make_unique<VerticalLayout>();
        m_border = std::make_unique<DownBorder>();
    }

    void Update(State &state);

    virtual void OnUpdate(State &state)
    {
        // state.graphics.FillRect(Color::BLUE, GetPosition().x, GetPosition().y, GetSize().x, GetSize().y);
    }

    void AddChild(std::unique_ptr<Panel> child);

    void SetLayoutStrategy(std::unique_ptr<LayoutStrategy> layout_strategy) {
        m_layout_strategy = std::move(layout_strategy);
    }
};

struct Label : public Panel
{
    std::string text = "";
    unsigned font_size = 8;
    Label(unsigned width, unsigned height) : Panel(width, height)
    {
    }

    void OnUpdate(State &state) override
    {
        Font &font = state.static_context.fonts.GetFont("default");
        FontGraphics font_graphics(state.graphics, font);
        font_graphics.SetDisplayWidth(font_size);
        Vector2 draw_dimensions = font_graphics.MeasureString(text);
        unsigned center_x = GetPosition().x + (GetSize().x / 2);
        unsigned draw_x = center_x - (draw_dimensions.x / 2);
        unsigned draw_y = GetPosition().y + (GetSize().y / 2) - (draw_dimensions.y / 2);
        font_graphics.DrawString(Color::WHITE, text, draw_x, draw_y);

        state.graphics.DrawRect(Color::GREEN, GetPosition().x, GetPosition().y, GetSize().x, GetSize().y);
    }
};

struct TextArea : public Panel
{
    // TODO: Add focus mechanic.  I think an event stream with consumable events is the way to go.
    TextType text_type;
    PulseTimer cursor_timer = PulseTimer(1.0f);
    Debouncer scroll_up_debouncer = Debouncer(Key::Up);
    Debouncer scroll_down_debouncer = Debouncer(Key::Down);

    Pallette pallette;

    char cursor_character = '_';
    unsigned padding = 5;
    unsigned scroll_pixel_offset = 0;

    TextArea(unsigned width, unsigned height) : Panel(width, height)
    {
    }

    void OnUpdate(State &state) override
    {
        Font &font = state.static_context.fonts.GetFont("default");
        FontGraphics font_graphics(state.graphics, font);
        font_graphics.SetDisplayWidth(8);

        text_type.Update(state.input, state.chrono);
        std::string text = text_type.GetText();

        scroll_up_debouncer.Update(state.input, state.chrono);
        if (state.input.IsPressed(Key::Up) || scroll_up_debouncer.IsHeld())
        {
            scroll_pixel_offset -= 1;
        }

        scroll_down_debouncer.Update(state.input, state.chrono);
        if (state.input.IsPressed(Key::Down) || scroll_down_debouncer.IsHeld())
        {
            scroll_pixel_offset += 1;
        }

        if (scroll_pixel_offset < 0)
        {
            scroll_pixel_offset = 0;
        }
        

        unsigned x = GetPosition().x;
        unsigned y = GetPosition().y;
        unsigned width = GetSize().x;
        unsigned height = GetSize().y;

        // Draw the background
        // state.graphics.FillRect(pallette.background, x, y, width, height);
        state.graphics.DrawRect(pallette.border, x, y, width, height);

        TextRenderer text_render(x + padding, y + padding, width - padding, height - padding,
                                 padding, scroll_pixel_offset);
        text_render.DrawText(pallette.foreground, text_type.GetText(), font_graphics);

        // Update the cursor and draw it to the screen
        cursor_character = cursor_character == ' ' ? '_' : ' ';
    

        std::string cursor_string = std::string(1, this->cursor_character);
        text_render.DrawText(pallette.foreground, cursor_string, font_graphics);
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

    void Update(FocusManager &manager, State &state, Window &parent, Pallette pallette, FontGraphics &font_graphics,
                std::string title);
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
        root_panel = std::make_unique<Panel>(w, h);
    }

    // TODO: Instead of passing the window manager, back it part of the static context of the engine.
    // TODO: Instead of passing the pallette, make it part of the static context of the engine as a unit of style config.
    void Update(FocusManager &manager, State &state, FontGraphics &font_graphics, Pallette pallette);

    virtual void OnUpdate(FocusManager &manager, State &state, FontGraphics &font_graphics, Pallette pallette) = 0;

    Panel &GetRoot()
    {
        return *root_panel;
    }

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

    void OnUpdate(FocusManager &manager, State &state, FontGraphics &font_graphics, Pallette pallette) override;
};

// TODO: Rename to window manager
class FocusManager
{
    std::vector<Window *> windows;   // all windows that are registered
    Window *active_window = nullptr; // the window that has focus
    bool has_given_focus = false;    // we will only fulfill one focus request per frame
  public:
    void Update(State &state, FontGraphics &font_graphics, Pallette pallette);
    void RegisterWindow(Window *window);
    void RequestFocus(Window *window);
    bool HasFocus(Window *window);
};