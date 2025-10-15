#include "timer.h"

Timer::Timer()
    : m_startTime(), m_elapsed(0), m_running(false) {}

void Timer::start() {
    if (!m_running) {
        m_startTime = clock::now();
        m_running = true;
    }
}

void Timer::stop() {
    if (m_running) {
        m_elapsed += clock::now() - m_startTime;
        m_running = false;
    }
}

void Timer::reset() {
    m_elapsed = std::chrono::duration<double>(0);
    m_running = false;
}

bool Timer::isRunning() const {
    return m_running;
}

double Timer::elapsedSeconds() const {
    if (m_running) {
        return m_elapsed.count() + std::chrono::duration<double>(clock::now() - m_startTime).count();
    }
    return m_elapsed.count();
}