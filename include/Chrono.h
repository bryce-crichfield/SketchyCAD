#pragma once

#include <chrono>
#include <vector>

namespace Chrono
{

struct Duration
{
    u_int64_t m_nanoseconds;

  public:
    Duration() = default;
    Duration(u_int64_t nanoseconds) : m_nanoseconds(nanoseconds)
    {
    }

    static Duration GetNow()
    {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        uint64_t nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
        return Duration(nanoseconds);
    }

    static Duration Indefinite() {
        return Duration(0xFFFFFFFFFFFFFFFF);
    }

    static Duration Seconds(float seconds)
    {
        return Duration(seconds * 1000000000.0f);
    }

    static Duration Zero()
    {
        return Duration(0);
    }

    float AsSeconds() const
    {
        return m_nanoseconds / 1000000000.0f;
    }

    // Arithmetic operators
    Duration operator+(const Duration &other) const
    {
        return Duration(m_nanoseconds + other.m_nanoseconds);
    }

    Duration operator-(const Duration &other) const
    {
        return Duration(m_nanoseconds - other.m_nanoseconds);
    }

    Duration operator*(const Duration &other) const
    {
        return Duration(m_nanoseconds * other.m_nanoseconds);
    }

    Duration operator/(const Duration &other) const
    {
        return Duration(m_nanoseconds / other.m_nanoseconds);
    }

    // Comparison operators
    bool operator==(const Duration &other) const
    {
        return m_nanoseconds == other.m_nanoseconds;
    }

    bool operator!=(const Duration &other) const
    {
        return m_nanoseconds != other.m_nanoseconds;
    }

    bool operator<(const Duration &other) const
    {
        return m_nanoseconds < other.m_nanoseconds;
    }

    bool operator<=(const Duration &other) const
    {
        return m_nanoseconds <= other.m_nanoseconds;
    }

    bool operator>(const Duration &other) const
    {
        return m_nanoseconds > other.m_nanoseconds;
    }

    bool operator>=(const Duration &other) const
    {
        return m_nanoseconds >= other.m_nanoseconds;
    }
};

class Meter
{
    Duration m_start_time;
    Duration m_last_time;
    Duration m_delta_time;

  public:
    Meter()
    {
        m_start_time = Duration::GetNow();
        m_last_time = m_start_time;
        m_delta_time = Duration::Zero();
    }

    Duration GetTime() const
    {
        return m_last_time;
    }
    Duration GetDelta() const
    {
        return m_delta_time;
    }

    void Update()
    {
        Duration now = Duration::GetNow();
        m_delta_time = now - m_last_time;
        m_last_time = now;
    }
};
} // namespace Chrono
