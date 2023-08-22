#include <cstddef>
#include <gtest/gtest.h>
#include <helpers.hpp>
#include <iterator>
#include <list.hpp>
#include <utility>

using TestObj = helpers::Test;

class ListTest : public ::testing::Test {
public:
  List<TestObj> l1;

  ListTest() { l1 = List<TestObj>{TestObj{1}, TestObj{2}, TestObj{3}}; }

  void testListsEq(const List<TestObj>& l1, const List<TestObj>& l2) {
    EXPECT_EQ(l2.size(), l1.size());
    std::size_t index{0};
    for (const TestObj& i : l2) {
      EXPECT_EQ(i.num(), l1[index].num());
      ++index;
    }
  }

protected:
  void SetUp() override {}
};

TEST_F(ListTest, CtorCapacityDefault) {
  List<TestObj> l2(5, TestObj{2});
  EXPECT_EQ(l2.size(), 5);
  int index{0};
  for (const TestObj& i : l2) {
    EXPECT_EQ(i.num(), 2);
    ++index;
  }
  EXPECT_EQ(index, 5);
}

TEST_F(ListTest, CtorCapacity) {
  List<TestObj> l2(5);
  EXPECT_EQ(l2.size(), 5);
  int index{0};
  for (const TestObj& i : l2) {
    EXPECT_EQ(i.num(), 0);
    ++index;
  }
  EXPECT_EQ(index, 5);
}

TEST_F(ListTest, CtorIterator) {
  List<TestObj> l2{l1.begin(), l1.end()};
  testListsEq(l1, l2);
}

TEST_F(ListTest, copyCtor) {
  List<TestObj> l2{l1};
  testListsEq(l1, l2);
}

TEST_F(ListTest, copyAssignment) {
  List<TestObj> l2(2);
  l2 = l1;
  testListsEq(l1, l2);
}

TEST_F(ListTest, moveCtor) {
  List<TestObj> l2{std::move(l1)};
  EXPECT_EQ(l1.size(), 0);
}

TEST_F(ListTest, moveAssignment) {
  List<TestObj> l2(2);
  l2 = std::move(l1);
  EXPECT_EQ(l1.size(), 2);
}

TEST_F(ListTest, pushBackEmpty) {
  TestObj test{8};
  List<TestObj> l2{};
  l2.push_back(test);
  EXPECT_EQ(l2.size(), 1);
}

TEST_F(ListTest, pushBackCopy) {
  TestObj test{8};
  std::size_t size{l1.size()};
  l1.push_back(test);
  EXPECT_EQ(l1.size(), size + 1);
}

TEST_F(ListTest, pushBackMove) {
  TestObj test{8};
  std::size_t size{l1.size()};
  l1.push_back(std::move(test));
  EXPECT_EQ(l1.size(), size + 1);
}

TEST_F(ListTest, iteratorIncrement) {
  std::size_t index{0};
  for (const TestObj& i : l1) {
    EXPECT_EQ(l1[index].num(), i.num());
    ++index;
  }
  EXPECT_EQ(index, 3);
}

TEST_F(ListTest, iteratorDecrement) {
  std::size_t index{3};
  for (auto i = l1.rbegin(); i != l1.rend(); ++i) {
    --index;
    EXPECT_EQ(l1[index].num(), i->num());
  }
  EXPECT_EQ(index, 0);
}

TEST_F(ListTest, popBackPushBack) {
  const std::size_t size{l1.size()};
  for (std::size_t i{0}; i < size; ++i) {
    l1.pop_back();
  }
  for (int i{0}; i < 3; ++i) {
    TestObj test{i + 1};
    l1.push_back(test);
  }

  for (int i{0}; i < 3; ++i) {
    EXPECT_EQ(l1[i].num(), i + 1);
  }

  EXPECT_EQ(l1.size(), 3);

  l1.pop_back();
  l1.push_back(TestObj{3});
  l1.push_back(TestObj{4});
  EXPECT_EQ(l1.size(), 4);
  EXPECT_EQ(l1[2].num(), 3);
  EXPECT_EQ(l1[3].num(), 4);
}

TEST_F(ListTest, popFrontPushFront) {
  const std::size_t size{l1.size()};
  for (int i{0}; i < size; ++i) {
    l1.pop_front();
  }

  for (int i{0}; i < 3; ++i) {
    TestObj test{i + 1};
    l1.push_front(test);
  }

  std::size_t newSize{l1.size() - 1};
  for (auto iter{l1.rbegin()}; iter != l1.rend(); ++iter) {
    EXPECT_EQ(l1[newSize].num(), l1.size() - newSize);
    --newSize;
  }

  EXPECT_EQ(l1.size(), 3);

  l1.pop_front();
  l1.push_front(TestObj{3});
  l1.push_front(TestObj{4});
  EXPECT_EQ(l1.size(), 4);
  EXPECT_EQ(l1[0].num(), 4);
  EXPECT_EQ(l1[1].num(), 3);
}

TEST_F(ListTest, pushBack) {
  List<TestObj> l2{TestObj{4}, TestObj{5}, TestObj{6}};
  for (const TestObj& i : l2) {
    l1.push_back(i);
  }

  EXPECT_EQ(l1[3].num(), 4);
  EXPECT_EQ(l1[4].num(), 5);
  EXPECT_EQ(l1[5].num(), 6);

  EXPECT_EQ(l1.size(), 6);
}

TEST_F(ListTest, insert) {
  List<TestObj> l2{TestObj{4}, TestObj{6}, TestObj{7}};
  List<TestObj>::iterator insertPos{l2.begin()};
  ++insertPos;
  l2.insert(insertPos, TestObj{5});

  EXPECT_EQ(l2[1].num(), 5);
  EXPECT_EQ(l2.size(), 4);

  l2.insert(insertPos, l1.begin(), l1.end());

  EXPECT_EQ(l2.size(), 4 + l1.size());
  EXPECT_EQ(l2[2].num(), l1[0].num());
  EXPECT_EQ(l2[3].num(), l1[1].num());
  EXPECT_EQ(l2[4].num(), l1[2].num());
}

TEST_F(ListTest, erase) {
  List<TestObj> l2{TestObj{4}, TestObj{6}, TestObj{7}};
  List<TestObj>::iterator insertPos{l2.begin()};
  ++insertPos;
  l2.erase(insertPos);

  EXPECT_NE(l2[1].num(), 6);
  EXPECT_EQ(l2.size(), 2);

  insertPos = l2.begin();
  ++insertPos;
  l2.insert(insertPos, TestObj{5});
  l2.insert(insertPos, TestObj{8});
  EXPECT_EQ(l2.size(), 4);

  List<TestObj>::iterator erasePos{l2.begin()};
  ++erasePos;
  l2.erase(erasePos, l2.end());
  EXPECT_EQ(l2.size(), 1);
  EXPECT_EQ(l2[0].num(), 4);
}

TEST_F(ListTest, reverseIter) {
  List<TestObj> lBeforeReverse = l1;
  l1.reverse_iter();
  auto size = l1.size();
  for (std::size_t i{0}; i < size; ++i) {
    EXPECT_EQ(l1[i].num(), lBeforeReverse[(size - 1)- i].num());
  }
}

TEST_F(ListTest, reverseRecursion) {
  List<TestObj> lBeforeReverse = l1;
  l1.reverse_recursive();
  auto size = l1.size();
  for (std::size_t i{0}; i < size; ++i) {
    EXPECT_EQ(l1[i].num(), lBeforeReverse[(size - 1)- i].num());
  }
}