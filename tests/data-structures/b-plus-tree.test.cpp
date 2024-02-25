#include <array>
#include <gtest/gtest.h>
#include <helpers.hpp>
#include <iostream>
#include <random.hpp>
#include <string>
#include <string_view>
#include <timer.hpp>
#include <tree.hpp>
#include <utility>
#include <vector.hpp>

class ContainerTest : public ::testing::Test {
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

  void compareBtreeEqualFlat(const Vector<int>& flatBTree) {
    std::size_t i{};
    _btree.walk_depth_first_inorder([&i, &flatBTree](int, helpers::Test& data) {
      EXPECT_EQ(data.num(), flatBTree[i]);
      ++i;
    });
    EXPECT_EQ(i, flatBTree.size());
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

class ContainerDeleteTest : public ::testing::Test {
public:
  trees::BPlusTree<int, helpers::Test, 2> _btree{};
  Vector<int> _flatBTree{5, 15, 20, 25, 30, 35, 40, 45, 55};

  void compareBtreeEqualFlat() {
    std::size_t i{};
    _btree.walk_depth_first_inorder([this, &i](int, helpers::Test& data) {
      EXPECT_EQ(data.num(), this->_flatBTree[i]);
      ++i;
    });
    EXPECT_EQ(i, this->_flatBTree.size());
  }

  void compareBtreeEqualFlat(const Vector<int>& flatBTree) {
    std::size_t i{};
    _btree.walk_depth_first_inorder([&i, &flatBTree](int, helpers::Test& data) {
      EXPECT_EQ(data.num(), flatBTree[i]);
      ++i;
    });
    EXPECT_EQ(i, flatBTree.size());
  }

protected:
  void SetUp() override {
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    _btree.insert(25, helpers::Test{25});
    _btree.insert(30, helpers::Test{30});
    _btree.insert(35, helpers::Test{35});
    _btree.insert(40, helpers::Test{40});
    _btree.insert(45, helpers::Test{45});
    _btree.insert(55, helpers::Test{55});
    _btree.insert(5, helpers::Test{5});
    _btree.insert(15, helpers::Test{15});
    _btree.insert(20, helpers::Test{20});

    compareBtreeEqualFlat();

    // tree would look like
    /*
                              35
        15             30             40
    5     15 20 25        30     35       40 45 55
    */
  };
};

TEST_F(ContainerTest, BPlusTreeCopy) {
  trees::BPlusTree<int, helpers::Test, 3> tree{_btree};
  std::size_t index{};
  tree.walk_depth_first_inorder(
      [&index, this](const int&, helpers::Test& data) {
        EXPECT_EQ(data.num(), _flatBTree[index]);
        ++index;
      });
}

TEST_F(ContainerTest, BPlusTreeSearch) {
  helpers::Test* result = _btree.search(32);
  if (result) {
    EXPECT_EQ((*result).num(), 32);
  } else {
    FAIL();
  }
}

TEST_F(ContainerTest, BPlusTreeSearchRange) {
  auto result = _btree.search(18, 57);
  EXPECT_EQ(result.size(), 16);
  std::array range{19, 21, 22, 23, 25, 27, 30, 31,
                   32, 39, 40, 41, 47, 50, 55, 56};
  for (std::size_t i{}; i < result.size(); ++i) {
    EXPECT_EQ(result[i].second.num(), range[i]);
  }

  result = _btree.search(17, 60);
  std::array range2{17, 19, 21, 22, 23, 25, 27, 30, 31,
                    32, 39, 40, 41, 47, 50, 55, 56, 60};
  EXPECT_EQ(result.size(), 18);
  for (std::size_t i{}; i < result.size(); ++i) {
    EXPECT_EQ(result[i].second.num(), range2[i]);
  }

  result = _btree.search(1, 90);
  EXPECT_EQ(result.size(), _flatBTree.size());
  for (std::size_t i{}; i < result.size(); ++i) {
    EXPECT_EQ(result[i].second.num(), _flatBTree[i]);
  }
}

TEST_F(ContainerTest, BPlusTreeHeight) { EXPECT_EQ(_btree.height(), 2); }

TEST_F(ContainerTest, BPlusTreeMin) {
  auto valuePair{_btree.min()};
  EXPECT_EQ(valuePair.first, 1);
  EXPECT_EQ(valuePair.second.num(), 1);
}

TEST_F(ContainerTest, BPlusTreeMax) {
  auto valuePair{_btree.max()};
  EXPECT_EQ(valuePair.first, 90);
  EXPECT_EQ(valuePair.second.num(), 90);
}

TEST_F(ContainerTest, BplusTreeDeleteAll) {
  for (int i{static_cast<int>(_flatBTree.size() - 1)}; i >= 0; --i) {
    _btree.remove(_flatBTree[static_cast<std::size_t>(i)]);
    _flatBTree.pop_back();
    compareBtreeEqualFlat();
  }
  EXPECT_EQ(_btree.height(), 0);
}

TEST_F(ContainerDeleteTest, BPlusTreeDelete) {
  Vector flatBTree{5, 15, 25, 30, 35, 40, 45, 55};
  _btree.remove(20);

  EXPECT_EQ(_btree.search(20), nullptr);

  EXPECT_EQ(_btree.height(), 2);

  compareBtreeEqualFlat(flatBTree);

  _btree.remove(5);

  EXPECT_EQ(_btree.search(5), nullptr);

  EXPECT_EQ(_btree.height(), 2);

  flatBTree = {15, 25, 30, 35, 40, 45, 55};

  compareBtreeEqualFlat(flatBTree);

  _btree.remove(40);

  EXPECT_EQ(_btree.search(40), nullptr);

  EXPECT_EQ(_btree.height(), 2);

  flatBTree = {15, 25, 30, 35, 45, 55};

  compareBtreeEqualFlat(flatBTree);

  _btree.remove(15);

  EXPECT_EQ(_btree.search(15), nullptr);

  EXPECT_EQ(_btree.height(), 2);

  flatBTree = {25, 30, 35, 45, 55};

  compareBtreeEqualFlat(flatBTree);

  _btree.remove(35);

  EXPECT_EQ(_btree.search(35), nullptr);

  EXPECT_EQ(_btree.height(), 1);

  flatBTree = {25, 30, 45, 55};

  compareBtreeEqualFlat(flatBTree);

  _btree.remove(45);

  EXPECT_EQ(_btree.search(45), nullptr);

  EXPECT_EQ(_btree.height(), 1);

  flatBTree = {25, 30, 55};

  compareBtreeEqualFlat(flatBTree);

  _btree.remove(25);

  EXPECT_EQ(_btree.search(25), nullptr);

  EXPECT_EQ(_btree.height(), 1);

  flatBTree = {30, 55};

  compareBtreeEqualFlat(flatBTree);

  _btree.remove(30);

  EXPECT_EQ(_btree.search(30), nullptr);

  EXPECT_EQ(_btree.height(), 0);

  flatBTree = {55};

  compareBtreeEqualFlat(flatBTree);

  _btree.remove(55);

  EXPECT_EQ(_btree.search(55), nullptr);

  EXPECT_EQ(_btree.height(), 0);

  std::cout << _btree << "\n";
}

TEST(PerfTest, BPlusTree) {
  trees::BPlusTree<int, int, 3> tree{};
  Timer timer{};
  for (int i{}; i < 1000000; ++i) {
    tree.insert(i, i);
  }
  std::cout << timer.elapsed() << "\n";
  timer.reset();
  tree.search(6789);
}