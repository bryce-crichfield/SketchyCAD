#include <core/gui/InteractionLock.h>
#include <core/gui/Interactor.h>

#include <iostream>

namespace Core::Gui {

bool InteractionLock::IsLocked() const {
    return is_locked;
}

void InteractionLock::Notify() {
    for (auto &interactor : interactors) {
        std::cout << "Notifying interactor" << std::endl;
        interactor->OnNotify();
    }
}

void InteractionLock::Clear() {
    is_locked = false;
}

void InteractionLock::Lock(Interactor &interactor) {
    // Greedily acquire the lock
    is_locked = true;

    // Notify all interactors that the lock has been acquired
    for (auto &i : interactors) {
        i->has_lock = false;
    }
    interactor.has_lock = true;
}

void InteractionLock::Unlock(Interactor &interactor) {
    std::cout << "SETTING IS LOCKED FALSE" << std::endl;
    is_locked = false;
    interactor.has_lock = false;
}

void InteractionLock::Subscribe(Interactor* interactor) {
    interactors.push_back(interactor);
}

void InteractionLock::Unsubscribe(Interactor* interactor) {

}

Interactor::Interactor(InteractionLock& lock) : lock(lock) {
    lock.Subscribe(this);
}  

Interactor::~Interactor() {
    // lock.Unsubscribe(this);
}  

bool Interactor::HasLock() const {
    return has_lock;
}

bool Interactor::TryLock() {
    if (has_lock) {
        return true;
    }

    if (lock.IsLocked()) {
        return false;
    }

    lock.Lock(*this);
    return true;
}

void Interactor::ForceLock() {
    if (has_lock) {
        return;
    }

    lock.Lock(*this);
}

void Interactor::ReleaseLock() {
    if (!has_lock)
        return;
    lock.Unlock(*this);
}

void Interactor::OnNotify() {
    std::cout << "Interactor notified" << std::endl;
    has_lock = false;
}

}