#pragma

#include <core/chrono/Chronometer.h>

namespace Core {
class Clock : public Chronometer {
    Duration m_start_time;
    Duration m_last_time;

    Duration m_time;
    Duration m_delta;

  public:
    Clock() {
        Duration current_time = Duration::Now();
        m_start_time = current_time;
        m_last_time = m_start_time;
    }

    void Tick() {
        Duration current_time = Duration::Now();
        Duration delta_time = current_time - m_last_time;
        m_last_time = current_time;
        m_time = current_time - m_start_time;
        m_delta = delta_time;
    }

    Duration GetTime() const override {
        return m_time;
    }

    Duration GetDelta() const override {
        return m_delta;
    }
};
}