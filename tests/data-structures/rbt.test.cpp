#include <algorithm.hpp>
#include <array>
#include <gtest/gtest.h>
#include <helpers.hpp>
#include <iostream>
#include <random.hpp>
#include <string>
#include <string_view>
#include <tree.hpp>
#include <vector.hpp>

class ContainerTest : public ::testing::Test {
public:
  trees::RBT<int, helpers::Test> _bst{};

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
    _bst.push(1, helpers::Test{1});
    _bst.push(24, helpers::Test{24});
    _bst.push(25, helpers::Test{25});
    _bst.push(12, helpers::Test{12});
    _bst.push(0, helpers::Test{0});
    _bst.push(64, helpers::Test{64});
    _bst.push(30, helpers::Test{30});
    _bst.push(45, helpers::Test{45});
    _bst.push(50, helpers::Test{50});

    //                         [24] B
    //                   [1]B            [30]R
    //               [0]R  [12]R     [25]B  [50]B
    //                                    [45]R   [64]R
    std::cout << _bst;
  };
};

TEST_F(ContainerTest, BSTCopy) {
  trees::RBT<int, helpers::Test> tree{_bst};
  EXPECT_EQ(tree.is_binary_search_tree(), true);
  EXPECT_EQ(tree.height(), _bst.height());
  EXPECT_EQ(tree.max()->num(), _bst.max()->num());
  EXPECT_EQ(tree.min()->num(), _bst.min()->num());
}

TEST_F(ContainerTest, BSTSearch) {
  helpers::Test target{64};
  helpers::Test* result = _bst.search(64);
  if (result) {
    EXPECT_EQ(*result, target);
  } else {
    FAIL();
  }
}

TEST_F(ContainerTest, BSTMin) { EXPECT_EQ(_bst.min()->num(), 0); }

TEST_F(ContainerTest, BSTMax) { EXPECT_EQ(_bst.max()->num(), 64); }

TEST_F(ContainerTest, BSTHeight) { EXPECT_EQ(_bst.height(), 3); }

TEST_F(ContainerTest, RBY_LCA) {
  helpers::Test* result{_bst.lca(45, 25)};
  EXPECT_EQ(result->num(), 30);
}

TEST_F(ContainerTest, BST_walk_breadth_first) {
  std::array treeAsArr{24, 1, 30, 0, 12, 25, 50, 45, 64};
  std::size_t i{};
  _bst.walk_breadth_first(
      [&treeAsArr, &i](const int, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
}

TEST_F(ContainerTest, BST_walk_depth_first_preorder) {
  std::array treeAsArr{24, 1, 0, 12, 30, 25, 50, 45, 64};
  std::size_t i{};
  _bst.walk_depth_first_preorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
}

TEST_F(ContainerTest, BST_walk_depth_first_inorder) {
  std::array treeAsArr{0, 1, 12, 24, 25, 30, 45, 50, 64};
  std::size_t i{};
  _bst.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
}

TEST_F(ContainerTest, BST_walk_depth_first_postorder) {
  std::array treeAsArr{0, 12, 1, 25, 45, 64, 50, 30, 24};
  std::size_t i{};
  _bst.walk_depth_first_postorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
}

TEST_F(ContainerTest, BST_is_binary_search_tree) {
  EXPECT_EQ(_bst.is_binary_search_tree(), true);

  _bst.walk_depth_first_postorder([](const int&, helpers::Test& data) {
    data.setNum(Random::uniformRand(0, 100));
  });

  EXPECT_EQ(_bst.is_binary_search_tree(), true);
}

TEST_F(ContainerTest, BST_remove) {
  Vector<int> treeAsArr{0, 1, 12, 24, 25, 30, 45, 50};
  std::size_t i{};
  _bst.remove(64);
  EXPECT_EQ(_bst.max()->num(), 50);
  EXPECT_EQ(_bst.search(64), nullptr);

  _bst.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });

  std::cout << _bst << "\n";

  treeAsArr = {0, 1, 12, 24, 25, 45, 50};
  i = 0;

  _bst.remove(30);
  EXPECT_EQ(_bst.max()->num(), 50);
  EXPECT_EQ(_bst.search(30), nullptr);

  _bst.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });

  std::cout << _bst << "\n";

  treeAsArr = {0, 1, 12, 25, 45, 50};
  i = 0;

  _bst.remove(24);
  EXPECT_EQ(_bst.max()->num(), 50);
  EXPECT_EQ(_bst.search(24), nullptr);

  _bst.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });

  std::cout << _bst << "\n";

  _bst.push(60, helpers::Test{60});
  _bst.push(70, helpers::Test{70});
  _bst.push(30, helpers::Test{30});
  _bst.push(80, helpers::Test{80});

  treeAsArr = {0, 1, 12, 30, 45, 50, 60, 70, 80};
  i = 0;

  _bst.remove(25);

  _bst.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });

  std::cout << _bst << "\n";

  _bst.remove(80);
  _bst.remove(70);
  _bst.remove(60);
  _bst.remove(50);

  treeAsArr = {0, 1, 12, 30, 45};
  i = 0;
  _bst.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });

  std::cout << _bst << "\n";
  _bst.remove(12);

  treeAsArr = {0, 1, 30, 45};
  i = 0;
  _bst.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
  _bst.remove(45);

  treeAsArr = {0, 1, 30};
  i = 0;
  _bst.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
  _bst.remove(1);

  treeAsArr = {0, 30};
  i = 0;
  _bst.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
  _bst.remove(0);

  treeAsArr = {30};
  i = 0;
  _bst.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
  _bst.remove(30);

  std::cout << _bst << "\n";
}

TEST_F(ContainerTest, BST_inorder_successor_predecessor) {
  EXPECT_EQ(_bst.get_next_inorder(24)->num(), 25);
  EXPECT_EQ(_bst.get_previous_inorder(24)->num(), 12);
}