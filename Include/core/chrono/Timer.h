#pragma once

#include <core/chrono/Chronometer.h>

namespace Core {
// Oscillates between 0 and 1 with adjustable pulse duration.
class Timer {
    Duration duration;
    Duration current = Duration::FromMilliseconds(0);
    bool second_half = false;

  public:
    Timer(Duration duration) : duration(duration) {}

    bool Update(Chronometer& chrono)
    {
        current = current + chrono.GetDelta();
        bool is_halfway = current >= duration / 2;
        bool result = is_halfway && !second_half;
        if (is_halfway) second_half = true;
        if (current >= duration) {
            current = Duration(0);
            second_half = false;
        }
        return result;
    }
};
} // namespace Core