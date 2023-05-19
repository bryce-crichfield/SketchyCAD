#pragma once

#include <chrono>
#include <cstdint>

namespace Core {
class Duration {
    uint64_t nanoseconds;

  public:
    Duration() : nanoseconds(0) {}
    Duration(long long nanoseconds) : nanoseconds(nanoseconds) {}

    Duration operator+(Duration other) const { return Duration(nanoseconds + other.nanoseconds); }
    Duration operator-(Duration other) const { return Duration(nanoseconds - other.nanoseconds); }
    Duration operator*(Duration other) const { return Duration(nanoseconds * other.nanoseconds); }
    Duration operator/(Duration other) const { return Duration(nanoseconds / other.nanoseconds); }

    Duration operator*(float factor) const { return Duration(nanoseconds * factor); }
    Duration operator/(float factor) const { return Duration(nanoseconds / factor); }

    bool operator==(Duration other) const { return nanoseconds == other.nanoseconds; }
    bool operator!=(Duration other) const { return nanoseconds != other.nanoseconds; }

    bool operator<(Duration other) const { return nanoseconds < other.nanoseconds; }
    bool operator>(Duration other) const { return nanoseconds > other.nanoseconds; }
    bool operator<=(Duration other) const { return nanoseconds <= other.nanoseconds; }
    bool operator>=(Duration other) const { return nanoseconds >= other.nanoseconds; }

    float AsMilliseconds() const { return nanoseconds / 1000000.0f; }
    float AsSeconds() const { return nanoseconds / 1000000000.0f; }
    float AsMinutes() const { return nanoseconds / 60000000000.0f; }

    static Duration Now()
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
        return Duration(nanoseconds);
    }

    static Duration FromMilliseconds(u_int32_t milliseconds) { return Duration(milliseconds * 1000000); }
    static Duration FromSeconds(uint32_t seconds) { return Duration(seconds * 1000000000); }
};
}