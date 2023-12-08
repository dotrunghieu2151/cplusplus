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

TEST_F(ContainerTest, BPlustreeCopy) {
  trees::BPlusTree<int, helpers::Test, 3> tree{_btree};
  std::size_t index{};
  tree.walk_depth_first_inorder(
      [&index, this](const int&, helpers::Test& data) {
        EXPECT_EQ(data.num(), _flatBTree[index]);
        ++index;
      });
}

TEST_F(ContainerTest, BPlustreeSearch) {
  helpers::Test* result = _btree.search(32);
  if (result) {
    EXPECT_EQ((*result).num(), 32);
  } else {
    FAIL();
  }
}

TEST_F(ContainerTest, BPlustreeHeight) { EXPECT_EQ(_btree.height(), 2); }

TEST_F(ContainerTest, BPlustreeMin) {
  auto valuePair{_btree.min()};
  EXPECT_EQ(valuePair.first, 1);
  EXPECT_EQ(valuePair.second.num(), 1);
}

TEST_F(ContainerTest, BtreeMax) {
  auto valuePair{_btree.max()};
  EXPECT_EQ(valuePair.first, 90);
  EXPECT_EQ(valuePair.second.num(), 90);
}
