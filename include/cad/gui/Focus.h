#pragma once

namespace Cad {
struct Component;

struct FocusManager {
    static FocusManager& GetInstance() {
        static FocusManager instance;
        return instance;
    }

    Component* focused_component = nullptr;
    bool update_lock = false;

    void RequestFocus(Component& component);
    void ReleaseFocus(Component& component);
    Component* GetFocusedComponent();
    void Update() { update_lock = false; }
};ww
} // namespace Cad