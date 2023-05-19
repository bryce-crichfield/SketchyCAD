#pragma once

#include <core/chrono/Chronometer.h>

namespace Core {
// Oscillates between 0 and 1 with adjustable pulse duration.
class Timer {
    Duration duration;
    Duration current = Duration(0);

  public:
    Timer(Duration duration) : duration(duration) {}

    bool Update(Chronometer& chrono)
    {
        current = current + chrono.GetDelta();
        bool is_high = current > (duration * 0.5f);
        if (current >= duration)
            current = Duration(0);
        return is_high;
    }
};
}