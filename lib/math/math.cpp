#include <algorithm>
#include <cmath>
#include <math.hpp>
#include <vector.hpp>
#include <vector>
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

// time complexity O(sqrt(n))
// space complexity O(1)
// we deal with corner cases like: num = 1, 2, 3 and num divisible by 2 and 3
// for other num. Since a prime number can be expressed as 6k + 1, 6k - 1.
// we loop from i = 5, till i * i < n, increment by 6.
// we loop till square root of n because for every number, from 1 to its square
// root exists 1 prime number. if num mod i == 0 or num mod (i + 2) == 0 =>
// false
bool is_prime(int num) {
  // if num = 1 or 0
  if (num <= 1) {
    return false;
  }
  if (num == 2 || num == 3) {
    return true;
  }
  if (num % 2 == 0 || num % 3 == 0) {
    return false;
  }

  for (int i{5}; i * i <= num; i += 6) {
    if (num % i == 0 || num % (i + 2) == 0) {
      return false;
    }
  }
  return true;
}

// All primes smaller than or equal to
// n using Sieve of Eratosthenes. Given that n is a small number, i.e smaller
// than 10 million
// Time complexity: O(n*log(log(n)))
// Space complexity: O(n)
// not cache-friendly
Vector<int> find_prime(int n) {
  std::size_t max(n * 0.5 + 1);
  std::vector<bool> mark(max);
  mark[0] = 1;
  for (int i{3}; i * i <= n; i += 2) {
    if (mark[static_cast<std::size_t>(i * 0.5)] == 0) {
      for (int j{i * i}; j <= n; j += 2 * i) {
        mark[static_cast<std::size_t>(j * 0.5)] = 1;
      }
    }
  }
  Vector<int> result(max);
  for (int i{1}; i < n; ++i) {
    if (i == 2) {
      result.push_back(i);
    } else if (i % 2 == 1 && mark[static_cast<std::size_t>(i * 0.5)] == 0) {
      result.push_back(i);
    }
  }
  return result;
}

// Segmented version of find_prime
// Time complexity: O(n * ln(sqrt(n)))
// Space complexity: O(sqrt(n))
// cache-friendly
Vector<int> find_prime_segmented(int n) {
  // Compute all primes smaller than or equal
  // to square root of n find_prime
  constexpr int cacheLine{64 * 1024 / sizeof(int)};
  int limit{std::min<int>(std::floor(std::sqrt(n)), cacheLine)};
  Vector<int> primes{find_prime(limit)};

  // Divide the range [0..n-1] in different segments
  // We have chosen segment size as sqrt(n).
  int low{limit};
  int high{2 * limit};
  Vector<int> result(n * 0.5 + 1);
  result.insert(0, primes.begin(), primes.end());
  std::vector<bool> mark(static_cast<std::size_t>(limit * 0.5));

  // While all segments of range [0..n-1] are not processed,
  // process one segment at a time
  while (low < n) {
    if (high >= n) {
      high = n;
    }
    mark.assign(mark.size(), false);
    // Use the found primes by simpleSieve() to find
    // primes in current range
    for (std::size_t i{}; i < primes.size(); i++) {
      // Find the minimum number in [low..high] that is
      // a multiple of prime[i] (divisible by prime[i])
      // For example, if low is 31 and prime[i] is 3,
      // we start with 33.
      // we skip all even numbers
      if (primes[i] == 2) {
        continue;
      }
      int start{low / primes[i] * primes[i]};
      if (start < low) {
        start += primes[i];
      }
      if (start % 2 == 0) {
        start += primes[i];
      }
      /* Mark multiples of prime[i] in [low..high]:
          We are marking j - low for j, i.e. each number
          in range [low, high] is mapped to [0, high-low]
          so if range is [50, 100] marking 50 corresponds
          to marking 0, marking 51 corresponds to 1 and
          so on. In this way we need to allocate space only
          for range */
      for (int j{start}; j <= high; j += 2 * primes[i]) {
        mark[static_cast<std::size_t>((j - low) * 0.5)] = true;
      }
    }

    // Numbers which are not marked as false are prime
    int lowStart{low % 2 == 0 ? low + 1 : low};
    int highEnd{high % 2 == 0 ? high - 1 : high};
    for (int i{lowStart}; i <= highEnd; i += 2) {
      if (!mark[static_cast<std::size_t>((i - low) * 0.5)]) {
        result.push_back(i);
      }
    }

    // Update low and high for next segment
    low += limit;
    high += limit;
  }
  return result;
}

} // namespace math