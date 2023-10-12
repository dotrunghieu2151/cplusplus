#include <chrono>
#include <cstddef>
#include <gtest/gtest.h>
#include <helpers.hpp>
#include <simple-deque.hpp>
#include <thread>
#include <tracy/Tracy.hpp>

using TestObj = helpers::Test;

// ZoneScopedN("TEST DA");
// #define TRACY_NO_EXIT

// void* operator new(std ::size_t count) {
//   auto ptr = malloc(count);
//   TracyAlloc(ptr, count);
//   return ptr;
// }
// void operator delete(void* ptr) noexcept {
//   TracyFree(ptr);
//   free(ptr);
// }
class SimpleDequeTest : public ::testing::Test {
public:
  SimpleDeque<TestObj> queue;
  SimpleDequeTest() : queue{} {};

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
  void SetUp() override{
      // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  };
};

TEST_F(SimpleDequeTest, DefaultValueHolderFactory) {
  queue.push_back(TestObj{1});
  queue.push_front(TestObj{0});
  queue.push_back(TestObj{2});
  queue.push_back(TestObj{3});
  queue.push_back(TestObj{4});
  queue.push_back(TestObj{5});
  std::size_t index{0};
  for (const TestObj& i : queue) {
    EXPECT_EQ(i.num(), index);
    ++index;
  }
  EXPECT_EQ(index, queue.size());
}

TEST_F(SimpleDequeTest, listCtor) {
  SimpleDeque<TestObj> q{TestObj{0}, TestObj{1}, TestObj{2}};
  std::size_t index{0};
  for (const TestObj& i : q) {
    EXPECT_EQ(i.num(), index);
    ++index;
  }
  EXPECT_EQ(index, q.size());
}

TEST_F(SimpleDequeTest, copyCtor) {
  SimpleDeque<TestObj> q{TestObj{0}, TestObj{1}, TestObj{2}};
  std::size_t index{0};
  queue = q;
  for (const TestObj& i : q) {
    EXPECT_EQ(i.num(), queue[index].num());
    ++index;
  }
  EXPECT_EQ(queue.size(), q.size());
  queue.push_back(TestObj{3});
  queue.push_front(TestObj{4});
  queue.push_back(TestObj{10});
  q.push_back(TestObj{3});
  q.push_front(TestObj{4});
  EXPECT_EQ(queue[0].num(), 4);
  EXPECT_EQ(queue[4].num(), 3);
  EXPECT_EQ(queue[5].num(), 10);

  EXPECT_EQ(q[0].num(), 4);
  EXPECT_EQ(q[4].num(), 3);
}

TEST_F(SimpleDequeTest, moveCtor) {
  SimpleDeque<TestObj> q{TestObj{0}, TestObj{1}, TestObj{2}};
  std::size_t index{0};
  queue = std::move(q);
  for (const TestObj& i : queue) {
    EXPECT_EQ(index, i.num());
    ++index;
  }
  EXPECT_EQ(queue.size(), index);
  queue.push_back(TestObj{3});
  queue.push_front(TestObj{4});
  queue.push_back(TestObj{10});

  EXPECT_EQ(queue[0].num(), 4);
  EXPECT_EQ(queue[4].num(), 3);
  EXPECT_EQ(queue[5].num(), 10);
}

TEST_F(SimpleDequeTest, PushPop) {
  SimpleDeque<TestObj> q(5);

  EXPECT_EQ(q.size(), 0);
  q.push_back(TestObj{2});
  q.push_front(TestObj{1});
  q.push_back(TestObj{3});
  q.push_front(TestObj{0});
  q.push_back(TestObj{4});

  q.pop_back();
  q.pop_front();
  q.push_front(TestObj{0});
  q.push_back(TestObj{4});

  std::size_t index{0};
  for (const TestObj& i : q) {
    EXPECT_EQ(index, i.num());
    ++index;
  }
  EXPECT_EQ(q.size(), 5);
}

TEST_F(SimpleDequeTest, InsertOne) {
  SimpleDeque<TestObj> q(5);

  EXPECT_EQ(q.size(), 0);
  SimpleDeque<TestObj>::iterator iter{q.begin()};
  iter = q.insert(iter, TestObj{3});
  iter = q.insert(iter, TestObj{1});
  q.insert(q.end(), TestObj{4});
  q.insert(q.begin(), TestObj{0});
  q.insert(q.begin() + 2, TestObj{2});

  std::size_t index{0};
  for (const TestObj& i : q) {
    EXPECT_EQ(index, i.num());
    ++index;
  }
  EXPECT_EQ(q.size(), 5);

  q.insert(q.begin() + 2, TestObj{10});
  EXPECT_EQ(q[2].num(), 10);

  q.pop_front();

  q.insert(q.begin() + 2, TestObj{11});
  EXPECT_EQ(q[2].num(), 11);

  q.erase(q.begin() + 2);
  EXPECT_EQ(q[2].num(), 2);
}

TEST_F(SimpleDequeTest, InsertMany) {
  SimpleDeque<TestObj> q1(5);
  SimpleDeque<TestObj> q2{TestObj{0}, TestObj{1}, TestObj{2}};

  EXPECT_EQ(q1.size(), 0);
  SimpleDeque<TestObj>::iterator iter{q1.begin()};
  iter = q1.insert(iter, q2.begin(), q2.end());

  EXPECT_EQ(q1.size(), 3);

  std::size_t index{0};
  for (const TestObj& i : q1) {
    EXPECT_EQ(index, i.num());
    ++index;
  }
  q2 = SimpleDeque<TestObj>{TestObj{3}, TestObj{4}, TestObj{5}};
  q1.insert(q1.end(), q2.begin(), q2.end());

  index = 0;
  for (const TestObj& i : q1) {
    EXPECT_EQ(index, i.num());
    ++index;
  }
  EXPECT_EQ(q1.size(), 6);
  q2 = SimpleDeque<TestObj>{TestObj{12}, TestObj{13}, TestObj{14}};
  q1.insert(q1.begin() + 1, TestObj{11});
  EXPECT_EQ(q1.size(), 7);
  q1.insert(q1.begin() + 3, q2.begin(), q2.end());
  EXPECT_EQ(q1.size(), 10);
  EXPECT_EQ(q1[3].num(), 12);
}

TEST_F(SimpleDequeTest, EraseMany) {
  SimpleDeque<TestObj> q{
      TestObj{0}, TestObj{1}, TestObj{2}, TestObj{3}, TestObj{4},
  };

  q.erase(q.begin() + 1, q.end() - 1);

  EXPECT_EQ(q.size(), 2);
  EXPECT_EQ(q[0].num(), 0);
  EXPECT_EQ(q[1].num(), 4);
};