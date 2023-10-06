#include <circular-buffer.hpp>
#include <cstddef>
#include <gtest/gtest.h>
#include <helpers.hpp>

using TestObj = helpers::Test;
class CircularBufferTest : public ::testing::Test {
public:
  CircularBuffer<TestObj, 5> buffer1;
  CircularBufferTest() : buffer1{} {}

  void testBufferEq(const CircularBuffer<TestObj, 5>& buff1,
                    const CircularBuffer<TestObj, 5>& buff2) {
    EXPECT_EQ(buff1.size(), buff2.size());
    std::size_t index{0};
    for (const TestObj& i : buff2) {
      EXPECT_EQ(i.num(), buff1[index].num());
      ++index;
    }
  }

protected:
  void SetUp() override{};
};

TEST_F(CircularBufferTest, CtorDefaultValue) {
  CircularBuffer<TestObj, 5> buff{TestObj{0}};
  EXPECT_EQ(buff.size(), 5);
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), 0);
  }
}

TEST_F(CircularBufferTest, copyCtor) {
  CircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buffer1 = buff;
  EXPECT_EQ(buffer1.size(), 1);
  buffer1.push_back(TestObj{2});
  EXPECT_EQ(buffer1.size(), 2);
}

TEST_F(CircularBufferTest, moveCtor) {
  CircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buffer1 = std::move(buff);
  EXPECT_EQ(buffer1.size(), 1);
  buffer1.push_back(TestObj{2});
  EXPECT_EQ(buffer1.size(), 2);
}

TEST_F(CircularBufferTest, Iterator) {
  CircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{2});
  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), index);
    ++index;
  }
  EXPECT_EQ(buff.size(), 3);
  EXPECT_EQ(index, 3);

  CircularBuffer<TestObj, 5> buff2{TestObj{0}};
  index = 0;
  for (const TestObj& i : buff2) {
    EXPECT_EQ(i.num(), 0);
    ++index;
  }
  EXPECT_EQ(buff2.size(), index);
}