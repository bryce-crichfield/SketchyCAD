#pragma once

#include <cad/gui/Focus.h>
#include <cad/gui/Component.h>

namespace Cad {
void FocusManager::RequestFocus(Component& component) {
    if (update_lock) return;

    if (focused_component == &component) return;

    if (focused_component != nullptr) {
        auto lost_focus_event = std::make_unique<FocusEvent>(FocusEvent::Type::Lost);
        focused_component->OnEvent(*lost_focus_event);
    }

    focused_component = &component;
    auto gained_focus_event = std::make_unique<FocusEvent>(FocusEvent::Type::Gained);
    focused_component->OnEvent(*gained_focus_event);
    update_lock = true;
}

void FocusManager::ReleaseFocus(Component& component) {
    if (focused_component == &component) {
        auto lost_focus_event = std::make_unique<FocusEvent>(FocusEvent::Type::Lost);
        focused_component->OnEvent(*lost_focus_event);
        focused_component = nullptr;
    }
}

Component* FocusManager::GetFocusedComponent() { return focused_component; }
} // namespace Cad