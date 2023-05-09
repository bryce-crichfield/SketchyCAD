#include <Window.h>

void TextRenderer::draw_glyph(Pixel color, FontGraphics &font_graphics, char c, unsigned draw_x, unsigned draw_y)
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

void TextRenderer::DrawText(Pixel color, std::string text, FontGraphics &font)
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