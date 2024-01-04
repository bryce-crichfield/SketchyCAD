#pragma once

#include <core/input/Debouncer.h>
#include <core/Controller.h>

namespace Core {

class Typist {
    std::string m_text_buffer;

    KeyDebouncer backspace_debouncer = KeyDebouncer(Key::Backspace);

    void do_backspace(Controller& controller)
    {
        auto &input = controller.GetInput();

        if (input.IsPressed(Key::Backspace) && m_text_buffer.size() > 0) {
            m_text_buffer.pop_back();
        }

        bool is_backspace = backspace_debouncer.Update(controller);
        if (is_backspace && m_text_buffer.size() > 0) {
            m_text_buffer.pop_back();
        }
    }

  public:
    Typist() = default;

    void Update(Controller& controller)
    {
        m_text_buffer += controller.GetInput().GetText();
        do_backspace(controller);
    }

    std::string GetText() { return m_text_buffer; }

    void Clear() { m_text_buffer.clear(); }
};

}