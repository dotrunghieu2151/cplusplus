#include <chrono>
#include <cstddef>
#include <deque.hpp>
#include <deque>
#include <gtest/gtest.h>
#include <helpers.hpp>
#include <iostream>
#include <timer.hpp>
#include <tracy/Tracy.hpp>

#define TRACY_NO_EXIT 1

using TestObj = helpers::Test;

// void* operator new(std ::size_t count) {
//   auto ptr = malloc(count);
//   TracyAlloc(ptr, count);
//   return ptr;
// }
// void operator delete(void* ptr) noexcept {
//   TracyFree(ptr);
//   free(ptr);
// }

class ContainerTest : public ::testing::Test {
public:
  Deque<TestObj> _container;
  ContainerTest() : _container{Deque<TestObj>(15)} {};

  void containersEq(const Deque<TestObj>& c1, const Deque<TestObj>& c2) {
    EXPECT_EQ(c1.size(), c2.size());
    std::size_t index{0};
    for (const TestObj& i : c2) {
      EXPECT_EQ(i.num(), c1[index].num());
      ++index;
    }
  }

protected:
  void SetUp() override{
      // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  };
};

TEST_F(ContainerTest, PushBack) {
  Deque<TestObj> c(2);
  for (int i{}; i < 15; ++i) {
    c.push_back(TestObj{i});
  }
  EXPECT_EQ(c.size(), 15);
  std::size_t index{0};
  for (const TestObj& i : c) {
    EXPECT_EQ(i.num(), c[index].num());
    ++index;
  }
  EXPECT_EQ(c.size(), index);
}

TEST_F(ContainerTest, PushBackExtends) {
  Deque<TestObj> c(2);
  // TC: push back without spare blocks
  for (int i{}; i < 15; ++i) {
    c.push_back(TestObj{i});
  }
  EXPECT_EQ(c.size(), 15);
  std::size_t index{0};
  for (const TestObj& i : c) {
    EXPECT_EQ(c[index].num(), i.num());
    ++index;
  }
  EXPECT_EQ(c.size(), index);

  // TC: push back with spared blocks and no allocation
  for (int i{}; i < 6; ++i) {
    c.pop_back();
  }

  for (int i{9}; i < 15; ++i) {
    c.push_back(TestObj{i});
  }

  index = 0;
  for (const TestObj& i : c) {
    EXPECT_EQ(c[index].num(), i.num());
    ++index;
  }
  EXPECT_EQ(c.size(), index);

  // TC: right > no spare blocks > push back and steal spared blocks on the left
  // and allocation
  for (int i{9}; i < 15; ++i) {
    c.push_front(TestObj{i});
  }

  for (int i{9}; i < 15; ++i) {
    c.pop_front();
  }

  c.push_front(TestObj{12});

  for (int i{15}; i < 20; ++i) {
    c.push_back(TestObj{i});
  }

  index = 0;
  for (const TestObj& i : c) {
    EXPECT_EQ(c[index].num(), i.num());
    ++index;
  }
  EXPECT_EQ(c.size(), index);
}

TEST_F(ContainerTest, PushFrontExtends) {
  Deque<TestObj> c(2);

  // TC: push front without spare blocks
  for (int i{14}; i >= 0; --i) {
    c.push_front(TestObj{i});
  }
  EXPECT_EQ(c.size(), 15);
  std::size_t index{0};
  for (const TestObj& i : c) {
    EXPECT_EQ(c[index].num(), i.num());
    ++index;
  }
  EXPECT_EQ(c.size(), index);

  // TC: push front with spared blocks and no allocation
  for (int i{}; i < 6; ++i) {
    c.pop_front();
  }

  for (int i{9}; i < 15; ++i) {
    c.push_front(TestObj{i});
  }

  index = 0;
  for (const TestObj& i : c) {
    EXPECT_EQ(c[index].num(), i.num());
    ++index;
  }
  EXPECT_EQ(c.size(), index);

  // TC: left > no spare blocks > push front and steal spared blocks on the left
  // and allocation
  for (int i{9}; i < 15; ++i) {
    c.push_back(TestObj{i});
  }

  for (int i{9}; i < 15; ++i) {
    c.pop_back();
  }

  c.push_back(TestObj{12});

  for (int i{15}; i < 20; ++i) {
    c.push_front(TestObj{i});
  }

  index = 0;
  for (const TestObj& i : c) {
    EXPECT_EQ(c[index].num(), i.num());
    ++index;
  }
  EXPECT_EQ(c.size(), index);
}

TEST_F(ContainerTest, PushFront) {
  Deque<TestObj> c(2);
  for (int i{14}; i >= 0; --i) {
    c.push_front(TestObj{i});
  }
  EXPECT_EQ(c.size(), 15);
  std::size_t index{0};
  for (const TestObj& i : c) {
    EXPECT_EQ(c[index].num(), i.num());
    ++index;
  }
  EXPECT_EQ(c.size(), index);
}

TEST_F(ContainerTest, PopBack) {
  Deque<TestObj> c(2);
  for (int i{14}; i >= 0; --i) {
    c.push_front(TestObj{i});
  }

  EXPECT_EQ(c.size(), 15);
  EXPECT_EQ(c[c.size() - 1].num(), 14);

  int index{14};
  while (c.size()) {
    TestObj ele{c.pop_back()};
    EXPECT_EQ(ele.num(), index);
    --index;
  }
}

TEST_F(ContainerTest, PopFront) {
  Deque<TestObj> c(2);
  for (int i{}; i < 15; ++i) {
    c.push_back(TestObj{i});
  }
  EXPECT_EQ(c.size(), 15);
  EXPECT_EQ(c[c.size() - 1].num(), 14);

  int index{14};
  while (c.size()) {
    TestObj ele{c.pop_back()};
    EXPECT_EQ(ele.num(), index);
    --index;
  }
}

TEST(PerfTest, StdDeque) {
  // ZoneScopedN("TEST StdDeque");
  Timer timer{};
  std::deque<TestObj> queue{};
  for (int i{}; i < 50000; ++i) {
    queue.push_back(TestObj{i});
  }
  for (int i{}; i < 50000; ++i) {
    queue.push_front(TestObj{i});
  }
  EXPECT_EQ(queue.size(), 100000);
}

TEST(PerfTest, CustomDeque) {
  // ZoneScopedN("TEST CustomDeque");
  Timer timer{};
  Deque<TestObj> queue{};
  for (int i{}; i < 50000; ++i) {
    queue.push_back(TestObj{i});
  }
  for (int i{}; i < 50000; ++i) {
    queue.push_front(TestObj{i});
  }
  EXPECT_EQ(queue.size(), 100000);
}