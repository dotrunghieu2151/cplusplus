#include <algorithm>
#include <cmath>
#include <math.hpp>

namespace math {
bool isFloatingPointEqualRelative(double a, double b, double relEpsilon) {
  return (std::abs(a - b) <= (std::max(std::abs(a), std::abs(b)) * relEpsilon));
};
bool isFloatingPointEqual(double a, double b, double absEpsilon,
                          double relEpsilon) {
  // Check if the numbers are really close -- needed when comparing numbers near
  // zero.
  double diff{std::abs(a - b)};
  if (diff <= absEpsilon)
    return true;

  // Otherwise fall back to Knuth's algorithm
  return isFloatingPointEqualRelative(a, b, relEpsilon);
};
} // namespace math