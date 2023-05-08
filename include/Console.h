#pragma once

#include "Engine.h"

struct Timer
{
    Counter counter;

    Timer(float duration) : counter(0.0f, duration)
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
    Timer backspace_timer = Timer(0.325f);

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

struct Rect
{
    unsigned x;
    unsigned y;
    unsigned width;
    unsigned height;
};

class TextRenderer
{
    unsigned x, y, width, height;
    unsigned padding;

    unsigned draw_x = 0;
    unsigned draw_y = 0;

    void draw_glyph(Pixel color, FontGraphics &font_graphics, char c, unsigned draw_x, unsigned draw_y)
    {
        // this method won't draw characters outside the console window, this just keeps
        // the draw code a lot cleaner
        unsigned abs_x = x + padding + draw_x;
        unsigned abs_y = y + padding + draw_y;

        if (abs_x < x + padding || abs_x > x + width - padding)
        {
            return;
        }

        if (abs_y < y + padding || abs_y > y + height - padding)
        {
            return;
        }

        font_graphics.DrawGlyph(color, c, abs_x, abs_y);
    }

  public:
    TextRenderer(unsigned x, unsigned y, unsigned width, unsigned height, unsigned padding, unsigned scroll_offset)
        : x(x), y(y), width(width), height(height), padding(padding), draw_y(scroll_offset)
    {
    }

    void DrawText(Pixel color, std::string text, FontGraphics &font)
    {
        // Extract space-delimited word from the input text
        std::vector<std::string> words;
        std::string word_buffer;
        for (char c : text)
        {
            if (c == ' ')
            {
                words.push_back(word_buffer + ' ');
                word_buffer.clear();
            }
            else
            {
                word_buffer.push_back(c);
            }
        }
        words.push_back(word_buffer);

        // Draw each word and increment the draw_x and draw_y offsets on a per-word basis
        unsigned line_end = width - padding;
        unsigned char_height = font.GetDisplayWidth();

        for (std::string &word : words)
        {
            unsigned word_display_length = word.size() * font.GetDisplayWidth();
            unsigned word_display_end = draw_x + word_display_length;

            // if the word is too long to fit on the line, move to the next line
            if (word_display_end > line_end)
            {
                draw_x = 0;
                draw_y += char_height;
            }

            for (char character : word)
            {
                draw_glyph(color, font, character, draw_x, draw_y);

                draw_x += font.GetDisplayWidth();

                // if the character overflows the line, move to the next line
                if (draw_x > line_end)
                {
                    draw_x = 0;
                    draw_y += char_height;
                }
            }
        }
    }
};

class ConsolePanel
{
    TextType text_type;

    Timer cursor_timer = Timer(1.0f);
    Debouncer scroll_up_debouncer = Debouncer(Key::Up);
    Debouncer scroll_down_debouncer = Debouncer(Key::Down);

    unsigned title_bar_x = 0;
    unsigned title_bar_y = 0;
    unsigned title_bar_height = 20;
    unsigned scroll_pixel_offset = 0;
    bool is_being_dragged = false;

    void update_input(Input &input, Chronometer &chrono)
    {
        // @brief: update the various input devices attached to the console such as the scroll functionality and typing

        text_type.Update(input, chrono);

        scroll_up_debouncer.Update(input, chrono);
        if (input.IsPressed(Key::Up) || scroll_up_debouncer.IsHeld())
        {
            scroll_pixel_offset -= 1;
        }

        scroll_down_debouncer.Update(input, chrono);
        if (input.IsPressed(Key::Down) || scroll_down_debouncer.IsHeld())
        {
            scroll_pixel_offset += 1;
        }

        if (scroll_pixel_offset < 0)
        {
            scroll_pixel_offset = 0;
        }
    }

    void update_window(Input &input, Graphics &graphics)
    {
        // @brief: updates the window title and vieware position based on the dragging input

        // If the mouse is dragging in the title bar, move the console window
        bool is_held = input.IsHeld(MouseButton::LEFT) && input.IsHeld(Key::LControl);
        bool is_moving = input.GetMouseDeltaX() != 0 || input.GetMouseDeltaY() != 0;
        bool is_inside_x = input.GetMouseX() > title_bar_x && input.GetMouseX() < title_bar_x + width;
        bool is_inside_y = input.GetMouseY() > title_bar_y && input.GetMouseY() < title_bar_y + title_bar_height;
        bool is_inside = is_inside_x && is_inside_y;

        // This works much better because it causes the title bar to act stickily
        // Once we have detecting any action inside, stick the window to the mouse
        // Once mouse is released, unstick the window irrespective of mouse position
        if (!is_held)
        {
            is_being_dragged = false;
        }

        if (is_held && is_inside)
        {
            is_being_dragged = true;
        }

        if (is_being_dragged)
        {
            x = input.GetMouseX() - (width / 2);
            y = input.GetMouseY() + (title_bar_height / 2);
            x = std::clamp(x, 0, (int)(graphics.GetWidth() - width));
            y = std::clamp(y, (int)title_bar_height, (int)(graphics.GetHeight() - (height)));
            title_bar_x = x;
            title_bar_y = y - title_bar_height;
        }
    }

    void draw_window_background(Graphics &graphics)
    {
        graphics.FillRect(color_background, x, y, width, height);
    }

    void draw_title_bar(State state, FontGraphics &font_graphics)
    {
        state.graphics.FillRect(color_background, title_bar_x, title_bar_y, width, title_bar_height);
        state.graphics.DrawRect(color_border, title_bar_x, title_bar_y, width, title_bar_height);
        font_graphics.DrawString(color_foreground, "Console", title_bar_x + padding, title_bar_y + padding);
    }

    void draw_text_area(Chronometer &chrono, FontGraphics &font_graphics)
    {
        TextRenderer text_render(x + padding, y + title_bar_height + padding, width - padding,
                                 height - title_bar_height - padding, padding, scroll_pixel_offset);
        text_render.DrawText(color_foreground, text_type.GetText(), font_graphics);

        // Update the cursor and draw it to the screen
        if (cursor_timer.Update(chrono))
        {
            cursor_character = cursor_character == ' ' ? '_' : ' ';
        }
        std::string cursor_string = std::string(1, this->cursor_character);
        text_render.DrawText(color_foreground, cursor_string, font_graphics);
    }

    void draw_window_border(Graphics &graphics)
    {
        graphics.DrawRect(color_border, x, y, width, height);
    }

  public:
    Pixel color_foreground = Color::BLACK;
    Pixel color_background = Color::WHITE;
    Pixel color_border = Color::BLACK;
    Pixel color_title_bar = Color::WHITE;

    char cursor_character = '_';
    int x, y;
    unsigned width, height;
    unsigned padding = 5;

    ConsolePanel(unsigned x, unsigned y, unsigned width, unsigned height) : x(x), y(y), width(width), height(height)
    {
        title_bar_x = x;
        title_bar_y = y - title_bar_height;
    }

    void OnUpdate(State &state, FontGraphics &font_graphics)
    {
        update_input(state.input, state.chrono);
        update_window(state.input, state.graphics);

        draw_window_background(state.graphics);
        draw_title_bar(state, font_graphics);
        draw_text_area(state.chrono, font_graphics);
        draw_window_border(state.graphics);
    }

    void SetWidth(unsigned width)
    {
        this->width = width;
    }

    void SetHeight(unsigned height)
    {
        this->height = height;
    }
};