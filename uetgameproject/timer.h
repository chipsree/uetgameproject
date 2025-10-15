#pragma once

#include <chrono>

class Timer {
public:
    Timer();

    void start();
    void stop();
    void reset();
    bool isRunning() const;
    double elapsedSeconds() const;

private:
    using clock = std::chrono::steady_clock;
    std::chrono::time_point<clock> m_startTime;
    std::chrono::duration<double> m_elapsed;
    bool m_running;
};