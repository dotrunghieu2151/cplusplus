#pragma once

#include <vector.hpp>

namespace math {
namespace constants {
inline constexpr double relEpsilon = 1e-8;
inline constexpr double absEpsilon = 1e-12;
} // namespace constants
bool isFloatingPointEqualRelative(double a, double b,
                                  double absEpsilon = constants::relEpsilon);
bool isFloatingPointEqual(double a, double b,
                          double absEpsilon = constants::absEpsilon,
                          double relEpsilon = constants::relEpsilon);

bool is_prime(int num);

Vector<int> find_prime(int n);
Vector<int> find_prime_segmented(int n);

template <typename T, typename U> auto max(T x, U y) {
  return (x < y) ? y : x;
};

template <typename T, typename U> auto min(T x, U y) {
  return (x < y) ? x : y;
};
} // namespace math