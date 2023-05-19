#pragma once

#include <core/input/InputState.h>
#include <core/input/Input.h>
#include <core/graphics/Graphics.h>
#include <core/graphics/Image.h>

#include <memory>

namespace Core {
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
    void UpdateInput();
    
    std::unique_ptr<Input> GetInput();
    std::unique_ptr<Graphics> GetGraphics();

    bool IsOpen() const;
    unsigned GetWidth() const;
    unsigned GetHeight() const;
    void SetTitle(const std::string& title);
};

}