#include <algorithm.hpp>
#include <gtest/gtest.h>
#include <math.hpp>
#include <string>
#include <string_view>
#include <timer.hpp>
#include <vector.hpp>

TEST(ConvertInfixToPostFix, test) {
  EXPECT_EQ(algorithms::evaluate_arithmetic_expression(
                "22*3+(81/3^2^2 + 145-(243+42))"),
            -73);
}

TEST(PrimeNumber, SegementedFindAllPrimeLessThanN) {
  Vector<int> correctResults{2,  3,  5,  7,  11, 13, 17, 19, 23, 29, 31, 37, 41,
                             43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
  Vector<int> results{math::find_prime_segmented(100)};

  EXPECT_EQ(correctResults.size(), results.size());
  for (std::size_t index{}; index < correctResults.size(); ++index) {
    EXPECT_EQ(correctResults[index], results[index]);
    EXPECT_EQ(math::is_prime(results[index]), true);
  }
}

TEST(PrimeNumber, SimpleFindAllPrimeLessThanN) {
  Vector<int> correctResults{2,  3,  5,  7,  11, 13, 17, 19, 23, 29, 31, 37, 41,
                             43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
  Vector<int> results{math::find_prime(100)};

  EXPECT_EQ(correctResults.size(), results.size());
  for (std::size_t index{}; index < correctResults.size(); ++index) {
    EXPECT_EQ(correctResults[index], results[index]);
    EXPECT_EQ(math::is_prime(results[index]), true);
  }
}

TEST(PrimeNumber, FindAllPrimePerfTest) {
  Timer timer{};

  Vector<int> results2{math::find_prime_segmented(40000000)};
  double segmentedPrimeTime{timer.elapsed()};

  std::cout << "Segmented Sieve time: " << segmentedPrimeTime << "\n";

  timer.reset();

  Vector<int> results1{math::find_prime(40000000)};
  double simplePrimeTime{timer.elapsed()};

  std::cout << "Simple Sieve time: " << simplePrimeTime << "\n";

  EXPECT_EQ(results2.size(), results1.size());
  // EXPECT_EQ(simplePrimeTime, segmentedPrimeTime);
}
