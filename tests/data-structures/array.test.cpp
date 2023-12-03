#include <array.hpp>
#include <cstddef>
#include <gtest/gtest.h>
#include <helpers.hpp>
#include <iterator>
#include <utility>

using TestObj = helpers::Test;

class ArrayTest : public ::testing::Test {
public:
  Array<TestObj, 4> arr1;

  ArrayTest() {
    arr1 = Array<TestObj, 4>{TestObj{0}, TestObj{1}, TestObj{2}, TestObj{3}};
  }

  void testListsEq(const Array<TestObj, 4>& arr1,
                   const Array<TestObj, 4>& arr2) {
    EXPECT_EQ(arr1.size(), arr2.size());
    std::size_t index{0};
    for (const TestObj& i : arr2) {
      EXPECT_EQ(i.num(), arr1[index].num());
      ++index;
    }
  }

protected:
  void SetUp() override {}
};

TEST_F(ArrayTest, CtorDefault) {
  Array<TestObj, 2> arr2;
  EXPECT_EQ(arr2.size(), 2);
  int index{0};
  for (TestObj& i : arr2) {
    i = TestObj{index};
    EXPECT_EQ(arr2[index].num(), index);
    ++index;
  }
  EXPECT_EQ(index, 2);
}

TEST_F(ArrayTest, CtorList) {
  Array arr2{TestObj{0}, TestObj{1}};
  EXPECT_EQ(arr2.size(), 2);
  int index{0};
  for (const TestObj& i : arr2) {
    EXPECT_EQ(i.num(), index);
    ++index;
  }
  EXPECT_EQ(index, 2);
}

TEST_F(ArrayTest, copyCtor) {
  Array<TestObj, 4> arr2{arr1};
  testListsEq(arr1, arr2);
}

TEST_F(ArrayTest, copyAssignment) {
  Array<TestObj, 4> arr2;
  arr2 = arr1;
  testListsEq(arr1, arr2);
}

TEST_F(ArrayTest, moveCtor) {
  Array<TestObj, 4> arr2{std::move(arr1)};
  EXPECT_EQ(arr2.size(), 4);
}

TEST_F(ArrayTest, moveAssignment) {
  Array<TestObj, 4> arr2{};
  arr2 = std::move(arr1);
  EXPECT_EQ(arr2.size(), 4);
}

TEST_F(ArrayTest, iteratorIncrement) {
  std::size_t index{0};
  for (const TestObj& i : arr1) {
    EXPECT_EQ(arr1[index].num(), i.num());
    ++index;
  }
  EXPECT_EQ(index, 4);
}

TEST_F(ArrayTest, iteratorDecrement) {
  std::size_t index{4};
  for (auto i = arr1.rbegin(); i != arr1.rend(); ++i) {
    --index;
    EXPECT_EQ(arr1[index].num(), i->num());
  }
  EXPECT_EQ(index, 0);
}