#include <cad/gui/Event.h>

namespace Cad {
bool Event::IsHandled() const { return is_handled; }

void Event::SetHandled() { is_handled = true; }

MouseClickEvent::MouseClickEvent(float x, float y, Core::Mouse button) : x(x), y(y), button(button) {}

void MouseClickEvent::Handle(EventHandler& handler) { handler.Handle(*this); }

void MouseMoveEvent::Handle(EventHandler& handler) { handler.Handle(*this); }

void MouseDragEvent::Handle(EventHandler& handler) { handler.Handle(*this); }

void MouseScrollEvent::Handle(EventHandler& handler) { handler.Handle(*this); }

void MouseHoverEvent::Handle(EventHandler& handler) { handler.Handle(*this); }

void MouseEnterEvent::Handle(EventHandler& handler) { handler.Handle(*this); }

void MouseLeaveEvent::Handle(EventHandler& handler) { handler.Handle(*this); }

void KeyPressEvent::Handle(EventHandler& handler) { handler.Handle(*this); }

void KeyReleaseEvent::Handle(EventHandler& handler) { handler.Handle(*this); }

void KeyRepeatEvent::Handle(EventHandler& handler) { handler.Handle(*this); }

void TextInputEvent::Handle(EventHandler& handler) { handler.Handle(*this); }

void FocusEvent::Handle(EventHandler& handler) { handler.Handle(*this); }

EventStream::EventStream(Core::Input& input) {
    events.clear();

    float mx = input.GetMouseX();
    float my = input.GetMouseY();
    if (input.IsPressed(Core::Mouse::LEFT)) {
        auto event = std::make_unique<MouseClickEvent>(mx, my, Core::Mouse::LEFT);
        events.push_back(std::move(event));
    }

    if (input.IsPressed(Core::Mouse::RIGHT)) {
        auto event = std::make_unique<MouseClickEvent>(mx, my, Core::Mouse::RIGHT);
        events.push_back(std::move(event));
    }

    if (input.IsPressed(Core::Mouse::MIDDLE)) {
        auto event = std::make_unique<MouseClickEvent>(mx, my, Core::Mouse::MIDDLE);
        events.push_back(std::move(event));
    }

    float mdx = input.GetMouseDeltaX();
    float mdy = input.GetMouseDeltaY();
    if (mdx != 0 || mdy != 0) {
        auto event = std::make_unique<MouseMoveEvent>(mx, my, mdx, mdy);
        events.push_back(std::move(event));
    }

    float msdx = input.GetScrollDeltaX();
    float msdy = input.GetScrollDeltaY();
    if (msdx != 0 || msdy != 0) {
        auto event = std::make_unique<MouseScrollEvent>(mx, my, msdx, msdy);
        events.push_back(std::move(event));
    }

    // Check each key using a for loop
    auto end_key = static_cast<unsigned>(Core::Key::Unknown);
    for (unsigned key = 0; key < end_key; key++) {
        Core::Key casted_key = static_cast<Core::Key>(key);
        if (input.IsPressed(casted_key)) {
            auto event = std::make_unique<KeyPressEvent>(casted_key);
            events.push_back(std::move(event));
        }
        if (input.IsReleased(casted_key)) {
            auto event = std::make_unique<KeyReleaseEvent>(casted_key);
            events.push_back(std::move(event));
        }
        // Must be done with debouncers for each key
        // if (input.IsRepeated(casted_key)) {
        //     auto event = std::make_unique<KeyRepeatEvent>();
        //     events.push_back(std::move(event));
        // }
    }

    std::string input_text = input.GetText();
    if (!input_text.empty()) {
        auto event = std::make_unique<TextInputEvent>(input_text);
        events.push_back(std::move(event));
    }

    // todo: focus events
}

bool Inside(float x, float y, float width, float height, float px, float py) {
    return px >= x && px <= x + width && py >= y && py <= y + height;
}

bool EventStream::HasNext() const { return !events.empty(); }

std::unique_ptr<Event> EventStream::Next() {
    auto event = std::move(events.back());
    events.pop_back();
    return event;
}

} // namespace Cad