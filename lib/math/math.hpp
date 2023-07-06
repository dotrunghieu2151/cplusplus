#pragma once

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

template <typename T, typename U> auto max(T x, U y) {
  return (x < y) ? y : x;
};

template <typename T, typename U> auto min(T x, U y) {
  return (x < y) ? x : y;
};
} // namespace math