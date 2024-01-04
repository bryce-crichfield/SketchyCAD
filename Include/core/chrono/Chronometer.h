#pragma once

#include <core/chrono/Duration.h>

namespace Core {
struct Chronometer {
    virtual Duration GetTime() const = 0;
    virtual Duration GetDelta() const = 0;
};
}