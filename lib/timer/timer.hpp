#pragma once
#include <chrono>

class Timer {
private:
  using Clock = std::chrono::steady_clock;
  using Second = std::chrono::duration<double, std::ratio<1>>;

  std::chrono::time_point<Clock> m_begin{};

public:
  Timer();
  ~Timer();
  void reset();

  double elapsed() const;
};