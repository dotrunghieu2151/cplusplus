#include <deque.hpp>
#include <gtest/gtest.h>
#include <helpers.hpp>
#include <stack.hpp>
#include <stack>
#include <timer.hpp>
#include <vector.hpp>
#include <vector>

TEST(StackTest, PushPop) {
  Stack<helpers::Test> stack{};
  for (int i{}; i < 4; ++i) {
    stack.push(helpers::Test{i});
  }
  EXPECT_EQ(stack.size(), 4);
  EXPECT_EQ(stack.top().num(), 3);

  for (int i{3}; i >= 0; --i) {
    helpers::Test t{stack.pop()};
    EXPECT_EQ(t.num(), i);
  }
  EXPECT_EQ(stack.size(), 0);
}

TEST(StackTest, Copy) {
  Stack<helpers::Test> stack{};
  for (int i{}; i < 4; ++i) {
    stack.push(helpers::Test{i});
  }
  EXPECT_EQ(stack.size(), 4);
  EXPECT_EQ(stack.top().num(), 3);

  Stack<helpers::Test> stack2{};
  stack2 = stack;

  for (int i{3}; i >= 0; --i) {
    helpers::Test t{stack2.pop()};
    EXPECT_EQ(t.num(), i);
  }
  EXPECT_EQ(stack2.size(), 0);
  EXPECT_EQ(stack.size(), 4);
}

TEST(StackTest, Move) {
  Stack<helpers::Test> stack{};
  for (int i{}; i < 4; ++i) {
    stack.push(helpers::Test{i});
  }
  EXPECT_EQ(stack.size(), 4);
  EXPECT_EQ(stack.top().num(), 3);

  Stack<helpers::Test> stack2{};
  stack2 = std::move(stack);

  EXPECT_EQ(stack2.size(), 4);
  EXPECT_EQ(stack.size(), 0);

  for (int i{3}; i >= 0; --i) {
    helpers::Test t{stack2.pop()};
    EXPECT_EQ(t.num(), i);
  }
  EXPECT_EQ(stack2.size(), 0);
}

TEST(PerfTest, StdStackDeque) {
  Timer timer{};
  std::stack<helpers::Test> stack{};
  for (int i{}; i < 100000; ++i) {
    stack.push(helpers::Test{i});
  }

  while (stack.size()) {
    stack.pop();
  }
}

TEST(PerfTest, StdStackVector) {
  Timer timer{};
  std::stack<helpers::Test, std::vector<helpers::Test>> stack{};
  for (int i{}; i < 100000; ++i) {
    stack.push(helpers::Test{i});
  }

  while (stack.size()) {
    stack.pop();
  }
}

TEST(PerfTest, CustomStackDeque) {
  Timer timer{};
  Stack<helpers::Test> stack{};
  for (int i{}; i < 100000; ++i) {
    stack.push(helpers::Test{i});
  }

  while (stack.size()) {
    stack.pop();
  }
}

TEST(PerfTest, CustomStackVector) {
  Timer timer{};
  Stack<helpers::Test, Vector<helpers::Test>> stack{};
  for (int i{}; i < 100000; ++i) {
    stack.push(helpers::Test{i});
  }

  while (stack.size()) {
    stack.pop();
  }
}