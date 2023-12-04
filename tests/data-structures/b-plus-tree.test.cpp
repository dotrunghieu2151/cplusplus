#include <algorithm.hpp>
#include <array>
#include <gtest/gtest.h>
#include <helpers.hpp>
#include <iostream>
#include <random.hpp>
#include <string>
#include <string_view>
#include <tree.hpp>
#include <utility>
#include <vector.hpp>

class ContainerTest : public ::testing::Test {
public:
  trees::BPlusTree<int, helpers::Test, 2> _btree{};

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
    _btree.insert(0, helpers::Test{0});
    _btree.insert(1, helpers::Test{1});
    _btree.insert(2, helpers::Test{2});
    _btree.insert(3, helpers::Test{3});
    _btree.insert(4, helpers::Test{4});
    _btree.insert(5, helpers::Test{5});
    _btree.insert(6, helpers::Test{6});
    _btree.insert(7, helpers::Test{7});
    _btree.insert(8, helpers::Test{8});
    _btree.insert(9, helpers::Test{9});
  };
};

class ContainerDeleteTest : public ::testing::Test {
public:
  trees::BPlusTree<int, helpers::Test, 3> _btree{};
  Vector<int> _flatBTree{1,  9,  15, 17, 19, 21, 22, 23, 25, 27, 30, 31,
                         32, 39, 40, 41, 47, 50, 55, 56, 60, 63, 72, 90};

  void compareBtreeEqualFlat() {
    std::size_t i{};
    _btree.walk_depth_first_inorder([this, &i](int, helpers::Test& data) {
      EXPECT_EQ(data.num(), this->_flatBTree[i]);
      ++i;
    });
    EXPECT_EQ(i, this->_flatBTree.size());
  }

protected:
  void SetUp() override {
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    _btree.insert(90, helpers::Test{90});
    _btree.insert(15, helpers::Test{15});
    _btree.insert(22, helpers::Test{22});
    _btree.insert(30, helpers::Test{30});
    _btree.insert(40, helpers::Test{40});
    _btree.insert(72, helpers::Test{72});

    _btree.insert(1, helpers::Test{1});
    _btree.insert(9, helpers::Test{9});

    _btree.insert(23, helpers::Test{23});
    _btree.insert(25, helpers::Test{25});
    _btree.insert(27, helpers::Test{27});

    _btree.insert(55, helpers::Test{55});
    _btree.insert(63, helpers::Test{63});

    _btree.insert(56, helpers::Test{56});
    _btree.insert(60, helpers::Test{60});

    _btree.insert(31, helpers::Test{31});
    _btree.insert(32, helpers::Test{32});
    _btree.insert(39, helpers::Test{39});

    _btree.insert(17, helpers::Test{17});
    _btree.insert(19, helpers::Test{19});
    _btree.insert(21, helpers::Test{21});

    _btree.insert(41, helpers::Test{41});
    _btree.insert(47, helpers::Test{47});
    _btree.insert(50, helpers::Test{50});

    compareBtreeEqualFlat();

    // tree would look like
    /*
                              30
        15             23              39             55       63
    1 9    17 19 21 22    25 27   31 32   40 41 47 50    56 60    72 90
    */
  };
};

TEST_F(ContainerTest, BtreeCopy) {
  trees::BPlusTree<int, helpers::Test, 2> tree{_btree};
  int index{};
  tree.walk_depth_first_inorder([&index](const int&, helpers::Test& data) {
    EXPECT_EQ(data.num(), index);
    ++index;
  });
}

TEST_F(ContainerTest, BtreeWalkPreOrder) {
  std::array flatBtree{3, 1, 0, 2, 5, 4, 7, 6, 8, 9};
  std::size_t index{};
  _btree.walk_depth_first_preorder(
      [&index, &flatBtree](const int&, helpers::Test& data) {
        EXPECT_EQ(data.num(), flatBtree[index]);
        ++index;
      });
  EXPECT_EQ(index, flatBtree.size());
}

TEST_F(ContainerTest, BtreeWalkPostOrder) {
  std::array flatBtree{0, 2, 1, 4, 6, 8, 9, 5, 7, 3};
  std::size_t index{};
  _btree.walk_depth_first_postorder(
      [&index, &flatBtree](const int&, helpers::Test& data) {
        EXPECT_EQ(data.num(), flatBtree[index]);
        ++index;
      });
  EXPECT_EQ(index, flatBtree.size());
}

TEST_F(ContainerTest, BtreeSearch) {
  helpers::Test* result = _btree.search(7);
  if (result) {
    EXPECT_EQ((*result).num(), 7);
  } else {
    FAIL();
  }
}

TEST_F(ContainerTest, BtreeHeight) { EXPECT_EQ(_btree.height(), 2); }

TEST_F(ContainerTest, BtreeMin) {
  auto valuePair{_btree.min()};
  EXPECT_EQ(valuePair.first, 0);
  EXPECT_EQ(valuePair.second.num(), 0);
}

TEST_F(ContainerTest, BtreeMax) {
  auto valuePair{_btree.max()};
  EXPECT_EQ(valuePair.first, 9);
  EXPECT_EQ(valuePair.second.num(), 9);
}

TEST_F(ContainerDeleteTest, BtreeDelete) {
  EXPECT_EQ(_btree.height(), 2);
  // case 1: Remove key from leaf node with sufficient num of keys
  _btree.remove(21);

  EXPECT_EQ(_btree.search(21), nullptr);

  _flatBTree.erase(5);

  compareBtreeEqualFlat();

  // case 2a: Remove key from an internal node with sufficient left predecessor
  _btree.remove(23);

  EXPECT_EQ(_btree.search(23), nullptr);

  _flatBTree.erase(6);

  compareBtreeEqualFlat();

  // case 2c: Remove key from an internal node with both children having minimum
  // key => merge
  _btree.remove(30);

  EXPECT_EQ(_btree.search(30), nullptr);

  _flatBTree.erase(8);

  compareBtreeEqualFlat();

  EXPECT_EQ(_btree.height(), 2);

  // case 2b: Remove key from an internal node with sufficient right successor
  _btree.remove(22);

  EXPECT_EQ(_btree.search(22), nullptr);

  _flatBTree.erase(5);

  compareBtreeEqualFlat();

  // case 3a: Remove key while there is a ancestor node that has t-1 key
  // tree height shrinks
  _btree.remove(25);

  EXPECT_EQ(_btree.search(25), nullptr);

  _flatBTree.erase(5);

  compareBtreeEqualFlat();

  EXPECT_EQ(_btree.height(), 1);

  // case 3b: Remove key in leaf with minimum num of keys, steal from sibling
  _btree.remove(9);

  EXPECT_EQ(_btree.search(9), nullptr);

  _flatBTree.erase(1);

  compareBtreeEqualFlat();

  EXPECT_EQ(_btree.height(), 1);

  _btree.insert(30, helpers::Test{30});

  _flatBTree.insert(5, 30);

  compareBtreeEqualFlat();

  EXPECT_EQ(_btree.height(), 1);
}
