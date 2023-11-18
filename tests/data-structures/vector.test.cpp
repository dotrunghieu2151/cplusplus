#include <gtest/gtest.h>
#include <helpers.hpp>
#include <timer.hpp>
#include <vector.hpp>
#include <vector>

TEST(PerfTest, CustomVector) {
  Timer timer{};
  Vector<helpers::Test> v(10);
  for (int i{}; i < 100000; ++i) {
    v.push_back(helpers::Test{i});
  }
}

TEST(PerfTest, StdVector) {
  Timer timer{};
  std::vector<helpers::Test> v(10);
  for (int i{}; i < 100000; ++i) {
    v.push_back(helpers::Test{i});
  }
}