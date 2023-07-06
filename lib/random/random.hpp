#pragma once

#include <chrono>
#include <random>

namespace Random
{
  using std::mt19937;

  mt19937 init();
  extern mt19937 rng;
  int uniformRand(int min, int max);
}