#include <chrono>
#include <timer.hpp>

void Timer::reset()
{
  m_begin = Clock::now();
}

double Timer::elapsed() const
{
  return std::chrono::duration_cast<Second>(Clock::now() - m_begin).count();
}