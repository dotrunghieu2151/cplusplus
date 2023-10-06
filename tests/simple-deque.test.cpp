#include <cstddef>
#include <gtest/gtest.h>
#include <helpers.hpp>
#include <simple-deque.hpp>

using TestObj = helpers::Test;
class SimpleDequeTest : public ::testing::Test {
public:
  SimpleDeque<TestObj> queue;
  SimpleDequeTest() : queue{} {}

  void testBufferEq(const SimpleDeque<TestObj>& queue1,
                    const SimpleDeque<TestObj>& queue2) {
    EXPECT_EQ(queue1.size(), queue2.size());
    std::size_t index{0};
    for (const TestObj& i : queue2) {
      EXPECT_EQ(i.num(), queue1[index].num());
      ++index;
    }
  }

protected:
  void SetUp() override{};
};

TEST_F(SimpleDequeTest, listCtor) {
  SimpleDeque<TestObj> q{TestObj{0}, TestObj{1}, TestObj{2}};
  std::size_t index{0};
  for (const TestObj& i : q) {
    EXPECT_EQ(i.num(), index);
    ++index;
  }
  EXPECT_EQ(index, q.size());
}