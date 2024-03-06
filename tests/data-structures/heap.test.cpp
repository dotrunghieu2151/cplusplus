#include <array>
#include <gtest/gtest.h>
#include <heap.hpp>
#include <helpers.hpp>
#include <string>
#include <string_view>

class ContainerTest : public ::testing::Test {
public:
  MinHeap<helpers::Test> _minHeap{};
  MaxHeap<helpers::Test> _maxHeap{};

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
    _minHeap.push(helpers::Test{0});
    _minHeap.push(helpers::Test{100});
    _minHeap.push(helpers::Test{130});
    _minHeap.push(helpers::Test{140});
    _minHeap.push(helpers::Test{-25});
    _minHeap.push(helpers::Test{20});
    _minHeap.push(helpers::Test{44});
    _minHeap.push(helpers::Test{64});
    _minHeap.push(helpers::Test{74});

    _maxHeap.push(helpers::Test{0});
    _maxHeap.push(helpers::Test{100});
    _maxHeap.push(helpers::Test{130});
    _maxHeap.push(helpers::Test{140});
    _maxHeap.push(helpers::Test{-25});
    _maxHeap.push(helpers::Test{20});
    _maxHeap.push(helpers::Test{44});
    _maxHeap.push(helpers::Test{64});
    _maxHeap.push(helpers::Test{74});
  };
};

TEST_F(ContainerTest, HeapMin) {
  EXPECT_EQ(_minHeap.top().num(), -25);
  _minHeap.push(helpers::Test{-100});
  EXPECT_EQ(_minHeap.top().num(), -100);
  helpers::Test popped{_minHeap.pop()};
  EXPECT_EQ(_minHeap.top().num(), -25);
  EXPECT_EQ(popped.num(), -100);
  std::array ordered{-25, 0, 20, 44, 64, 74, 100, 130, 140};
  std::size_t index{0};
  while (!_minHeap.empty()) {
    int t{_minHeap.pop().num()};
    EXPECT_EQ(t, ordered[index]);
    ++index;
  }
}

TEST_F(ContainerTest, HeapMinRemove) {
  helpers::Test target{44};
  _minHeap.remove(target);
  std::array ordered{-25, 0, 20, 64, 74, 100, 130, 140};
  std::size_t index{0};
  while (!_minHeap.empty()) {
    int t{_minHeap.pop().num()};
    EXPECT_EQ(t, ordered[index]);
    ++index;
  }
}

TEST_F(ContainerTest, HeapMax) {
  EXPECT_EQ(_maxHeap.top().num(), 140);
  _maxHeap.push(helpers::Test{1000});
  EXPECT_EQ(_maxHeap.top().num(), 1000);
  helpers::Test popped{_maxHeap.pop()};
  EXPECT_EQ(_maxHeap.top().num(), 140);
  EXPECT_EQ(popped.num(), 1000);

  _maxHeap.push(helpers::Test{-1000});
  std::array ordered{140, 130, 100, 74, 64, 44, 20, 0, -25, -1000};
  std::size_t index{0};
  while (!_maxHeap.empty()) {
    int t{_maxHeap.pop().num()};
    EXPECT_EQ(t, ordered[index]);
    ++index;
  }
}

TEST_F(ContainerTest, HeapMaxRemove) {
  helpers::Test target{0};
  _maxHeap.remove(target);
  std::array ordered{140, 130, 100, 74, 64, 44, 20, -25};
  std::size_t index{0};
  while (!_maxHeap.empty()) {
    int t{_maxHeap.pop().num()};
    EXPECT_EQ(t, ordered[index]);
    ++index;
  }
}

TEST_F(ContainerTest, MinHeapifyArray) {
  std::array unordered{1, 2, 6, 7, 4, 5, 3, 9, 8};
  std::array ordered{1, 2, 3, 4, 5, 6, 7, 8, 9};
  MinHeap<int> heapified(unordered.begin(), unordered.end());
  std::size_t index{};
  while (!heapified.empty()) {
    int t{heapified.pop()};
    EXPECT_EQ(t, ordered[index]);
    ++index;
  }
}

TEST_F(ContainerTest, MaxHeapifyArray) {
  std::array unordered{1, 2, 6, 7, 4, 5, 3, 9, 8};
  std::array ordered{9, 8, 7, 6, 5, 4, 3, 2, 1};
  MaxHeap<int> heapified(unordered.begin(), unordered.end());
  std::size_t index{};
  while (!heapified.empty()) {
    int t{heapified.pop()};
    EXPECT_EQ(t, ordered[index]);
    ++index;
  }
}