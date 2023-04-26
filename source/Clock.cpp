#include <Engine.h>

Clock::Clock()
{
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = now.time_since_epoch();
    double current_time = elapsed_seconds.count();
    m_start_time = current_time;
    m_last_time = m_start_time;
}

// @returns (global time, delta time)
std::pair<float, float> Clock::Tick()
{
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = now.time_since_epoch();
    double current_time = elapsed_seconds.count();
    double delta_time = current_time - m_last_time;
    m_last_time = current_time;

    m_time = current_time - m_start_time;
    m_delta = delta_time;

    return {(float)(current_time - m_start_time), (float)delta_time};
}

double Clock::GetTime() const
{
    return m_time;
}

double Clock::GetDelta() const
{
    return m_delta;
}