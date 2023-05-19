#pragma once

#include <core/gui/Interactor.h>
#include <vector>

namespace Core::Gui {
class Interactor;
class InteractionLock {
    bool is_locked = false;
    std::vector<Interactor*> interactors;

  public:
    bool IsLocked() const;
    void Lock(Interactor& interactor);
    void Unlock(Interactor& interactor);
    void Clear();
    void Notify();

    void Subscribe(Interactor* interactor);
    void Unsubscribe(Interactor* interactor);
};
}