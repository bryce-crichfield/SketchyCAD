#pragma once

#include <core/Core.h>

namespace Cad {

class Event;
struct EventHandler;
struct EventStream;

class Event {
    bool is_handled = false;

  public:
    ~Event() = default;

    bool IsHandled() const;
    void SetHandled();
    virtual void Handle(EventHandler& handler) = 0;
};

struct MouseClickEvent : public Event {
    float x, y;
    Core::Mouse button;
    MouseClickEvent(float x, float y, Core::Mouse button);
    void Handle(EventHandler& handler) override;
};

struct MouseMoveEvent : public Event {
    float x, y, dx, dy;
    MouseMoveEvent(float x, float y, float dx, float dy) : x(x), y(y), dx(dx), dy(dy) {}
    void Handle(EventHandler& handler) override;
};

struct MouseDragEvent : public Event {
    float dx, dy;
    MouseDragEvent(float dx, float dy) : dx(dx), dy(dy) {}
    void Handle(EventHandler& handler) override;
};

struct MouseScrollEvent : public Event {
    float x, y, dx, dy;
    MouseScrollEvent(float x, float y, float dx, float dy) : x(x), y(y), dx(dx), dy(dy) {}
    void Handle(EventHandler& handler) override;
};

struct MouseHoverEvent : public Event {
    MouseHoverEvent() = default;
    void Handle(EventHandler& handler) override;
};

struct MouseEnterEvent : public Event {
    MouseEnterEvent() = default;
    void Handle(EventHandler& handler) override;
};

struct MouseLeaveEvent : public Event {
    MouseLeaveEvent() = default;
    void Handle(EventHandler& handler) override;
};

struct KeyPressEvent : public Event {
    Core::Key key;
    KeyPressEvent(Core::Key key) : key(key) {}
    void Handle(EventHandler& handler) override;
};

struct KeyReleaseEvent : public Event {
    Core::Key key;
    KeyReleaseEvent(Core::Key key) : key(key) {}
    void Handle(EventHandler& handler) override;
};

struct KeyRepeatEvent : public Event {
    Core::Key key;
    KeyRepeatEvent(Core::Key key) : key(key) {}
    void Handle(EventHandler& handler) override;
};

struct TextInputEvent : public Event {
    std::string text;
    TextInputEvent(std::string text) : text(text) {}
    void Handle(EventHandler& handler) override;
};

struct FocusEvent : public Event {
    enum Type {
        Gained,
        Lost
    };

    Type type;

    FocusEvent(Type type) : type(type) {}
    void Handle(EventHandler& handler) override;
};

struct EventHandler {
    virtual void Handle(MouseClickEvent& event) {}
    virtual void Handle(MouseMoveEvent& event) {}
    virtual void Handle(MouseDragEvent& event) {}
    virtual void Handle(MouseScrollEvent& event) {}
    virtual void Handle(MouseHoverEvent& event) {}
    virtual void Handle(MouseEnterEvent& event) {}
    virtual void Handle(MouseLeaveEvent& event) {}
    virtual void Handle(KeyPressEvent& event) {}
    virtual void Handle(KeyReleaseEvent& event) {}
    virtual void Handle(KeyRepeatEvent& event) {}
    virtual void Handle(TextInputEvent& event) {}
    virtual void Handle(FocusEvent& event) {}
};

struct EventStream {
    std::vector<std::unique_ptr<Event>> events;

    EventStream(Core::Input& input);

    bool HasNext() const;
    std::unique_ptr<Event> Next();
};

} // namespace Cad