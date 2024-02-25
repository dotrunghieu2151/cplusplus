#include <algorithm>
#include <array>
#include <gtest/gtest.h>
#include <helpers.hpp>
#include <priority-queue.hpp>
#include <queue>
#include <random.hpp>
#include <string>
#include <string_view>
#include <timer.hpp>

class ContainerTest : public ::testing::Test {
public:
  PriorityQueue<helpers::Test, std::less_equal<helpers::Test>> _minPQ{};
  PriorityQueue<helpers::Test, std::greater_equal<helpers::Test>> _maxPQ{};

  // void containersEq(const Deque<TestObj>& c1, const Deque<TestObj>& c2) {
  //   EXPECT_EQ(c1.size(), c2.size());
  //   std::size_t index{0};
  //   for (const TestObj& i : c2) {
  //     EXPECT_EQ(i.num(), c1[index].num());
  //     ++index;
  //   }
  // }

protected:
  void SetUp() override {
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    _minPQ.push(helpers::Test{0});
    _minPQ.push(helpers::Test{100});
    _minPQ.push(helpers::Test{130});
    _minPQ.push(helpers::Test{140});
    _minPQ.push(helpers::Test{-25});
    _minPQ.push(helpers::Test{20});
    _minPQ.push(helpers::Test{44});
    _minPQ.push(helpers::Test{64});
    _minPQ.push(helpers::Test{74});

    _maxPQ.push(helpers::Test{0});
    _maxPQ.push(helpers::Test{100});
    _maxPQ.push(helpers::Test{130});
    _maxPQ.push(helpers::Test{140});
    _maxPQ.push(helpers::Test{-25});
    _maxPQ.push(helpers::Test{20});
    _maxPQ.push(helpers::Test{44});
    _maxPQ.push(helpers::Test{64});
    _maxPQ.push(helpers::Test{74});
  };
};

TEST_F(ContainerTest, MinPQ) {
  EXPECT_EQ(_minPQ.top().num(), -25);
  _minPQ.push(helpers::Test{-100});
  EXPECT_EQ(_minPQ.top().num(), -100);
  helpers::Test popped{_minPQ.pop()};
  EXPECT_EQ(_minPQ.top().num(), -25);
  EXPECT_EQ(popped.num(), -100);
  std::array ordered{-25, 0, 20, 44, 64, 74, 100, 130, 140};
  std::size_t index{0};
  while (!_minPQ.empty()) {
    int t{_minPQ.pop().num()};
    EXPECT_EQ(t, ordered[index]);
    ++index;
  }
}

TEST_F(ContainerTest, MaxPQ) {
  EXPECT_EQ(_maxPQ.top().num(), 140);
  _maxPQ.push(helpers::Test{1000});
  EXPECT_EQ(_maxPQ.top().num(), 1000);
  helpers::Test popped{_maxPQ.pop()};
  EXPECT_EQ(_maxPQ.top().num(), 140);
  EXPECT_EQ(popped.num(), 1000);

  _maxPQ.push(helpers::Test{-1000});
  std::array ordered{140, 130, 100, 74, 64, 44, 20, 0, -25, -1000};
  std::size_t index{0};
  while (!_maxPQ.empty()) {
    int t{_maxPQ.pop().num()};
    EXPECT_EQ(t, ordered[index]);
    ++index;
  }
}

TEST(Perf, StdPQ) {
  Timer timer{};
  std::priority_queue<helpers::Test> queue{};
  for (int i{}; i < 100000; ++i) {
    queue.push(helpers::Test{i});
  }
  while (!queue.empty()) {
    queue.pop();
  }
}

TEST(Perf, CustomPQ) {
  Timer timer{};
  PriorityQueue<helpers::Test, std::greater_equal<helpers::Test>> queue{};
  for (int i{}; i < 100000; ++i) {
    queue.push(helpers::Test{i});
  }
  while (!queue.empty()) {
    queue.pop();
  }
}