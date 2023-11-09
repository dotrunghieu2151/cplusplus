#include <algorithm.hpp>
#include <gtest/gtest.h>
#include <string>
#include <string_view>

TEST(ConvertInfixToPostFix, test) {
  EXPECT_EQ(algorithms::evaluate_arithmetic_expression(
                "22*3+(81/3^2^2 + 145-(243+42))"),
            -73);
}