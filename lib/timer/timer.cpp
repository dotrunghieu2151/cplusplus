#include <chrono>
#include <iostream>
#include <timer.hpp>

Timer::Timer() : m_begin{Clock::now()} { std::cout << "Timer starts ! \n"; }
Timer::~Timer() { std::cout << "Timer ends: " << elapsed() << "\n"; }

void Timer::reset() { m_begin = Clock::now(); }

double Timer::elapsed() const {
  return std::chrono::duration_cast<Second>(Clock::now() - m_begin).count();
}