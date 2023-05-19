#pragma once

#include <core/gui/InteractionLock.h>

namespace Core::Gui {
class InteractionLock;

class Interactor {
    friend class InteractionLock;
    InteractionLock& lock;
    bool has_lock = false;

  public:
    Interactor(InteractionLock& lock); // subscribe to lock
    ~Interactor();                     // unsubscribe from lock

    bool HasLock() const;
    bool TryLock();
    void ForceLock();
    void ReleaseLock();
    void OnNotify();
};
} // namespace Core::Gui