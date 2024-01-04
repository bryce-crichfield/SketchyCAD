#pragma once

#include <core/font/Font.h>

#include <memory>
#include <unordered_map>

namespace Core {
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
        if (m_fonts.find(name) == m_fonts.end())
        {
            throw std::runtime_error("Font not found");
        }
        return *m_fonts[name];
    }
};
} // namespace Core