#include <array>
#include <fibonacci-heap.hpp>
#include <gtest/gtest.h>
#include <heap.hpp>
#include <helpers.hpp>
#include <iostream>
#include <string>
#include <string_view>

class ContainerTest : public ::testing::Test {
public:
  FibonacciHeap<int, helpers::Test> _heap{};

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
    _heap.insert(3, helpers::Test{3});
    _heap.insert(7, helpers::Test{7});
    _heap.insert(17, helpers::Test{17});
    _heap.insert(23, helpers::Test{23});
    _heap.insert(24, helpers::Test{24});
    _heap.insert(18, helpers::Test{18});
    _heap.insert(39, helpers::Test{39});
    _heap.insert(52, helpers::Test{52});
    _heap.insert(38, helpers::Test{38});
    _heap.insert(41, helpers::Test{41});
    _heap.insert(30, helpers::Test{30});
    _heap.insert(26, helpers::Test{26});
    _heap.insert(35, helpers::Test{35});
    _heap.insert(46, helpers::Test{46});
    _heap.insert(0, helpers::Test{0});

    _heap.extract_top();

    std::cout << _heap << "\n";

    // heap
    /*
        (3) -------> (26) -------> (17)
         |        _____|____       __|__
         |        |    |    |     |     |
        (7)    (30)   (38)  (35)  (18)  (23)
                     __|__    |     |
                  (39)  (41) (46) (24)
                   |
                  (52)
    */
  };
};

TEST_F(ContainerTest, FibonacciHeapInsert) {
  _heap.insert(0, helpers::Test{0});
  EXPECT_EQ(_heap.top()->num(), 0);

  _heap.insert(102, helpers::Test{102});
  EXPECT_EQ(_heap.find(102)->num(), 102);
}

TEST_F(ContainerTest, FibonacciHeapFind) {
  EXPECT_EQ(_heap.find(52)->num(), 52);
  EXPECT_EQ(_heap.find(522), nullptr);
}

TEST_F(ContainerTest, FibonacciHeapExtractTop) {
  int key{};
  helpers::Test value{};
  std::tie(key, value) = _heap.extract_top();
  EXPECT_EQ(key, 3);
  EXPECT_EQ(value.num(), 3);

  std::tie(key, value) = _heap.extract_top();
  EXPECT_EQ(key, 7);
  EXPECT_EQ(value.num(), 7);

  std::tie(key, value) = _heap.extract_top();
  EXPECT_EQ(key, 17);
  EXPECT_EQ(value.num(), 17);
}

TEST_F(ContainerTest, FibonacciHeapUpdateKey) {
  EXPECT_EQ(_heap.find(15), nullptr);

  // move 30 to 15, 26 is marked
  _heap.update_key(30, 15);

  EXPECT_EQ(_heap.find(15)->num(), 30);

  // move 41 to 5
  EXPECT_EQ(_heap.find(5), nullptr);

  // 38 is marked
  _heap.update_key(41, 5);

  EXPECT_EQ(_heap.find(5)->num(), 41);

  // move 39 to 2, 26 and 38 marked so they should be cut to the top list
  EXPECT_EQ(_heap.find(2), nullptr);

  _heap.update_key(39, 2);

  EXPECT_EQ(_heap.find(2)->num(), 39);

  auto [key, value] = _heap.extract_top();

  EXPECT_EQ(key, 2);
  EXPECT_EQ(value.num(), 39);
}

TEST_F(ContainerTest, FibonacciHeapDelete) {
  EXPECT_EQ(_heap.find(38)->num(), 38);

  _heap.delete_key(38);

  EXPECT_EQ(_heap.find(38), nullptr);
  EXPECT_EQ(_heap.top()->num(), 3);
}
