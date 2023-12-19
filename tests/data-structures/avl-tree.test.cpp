#include <algorithm.hpp>
#include <array>
#include <gtest/gtest.h>
#include <helpers.hpp>
#include <random.hpp>
#include <string>
#include <string_view>
#include <tree.hpp>

class ContainerTest : public ::testing::Test {
public:
  trees::AVLTree<int, helpers::Test> _bst{};

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
    _bst.push(-24, helpers::Test{-24});
    _bst.push(25, helpers::Test{25});
    _bst.push(12, helpers::Test{12});
    _bst.push(0, helpers::Test{0});
    _bst.push(64, helpers::Test{64});
    _bst.push(-12, helpers::Test{-12});
  };
};

TEST_F(ContainerTest, BSTCopy) {
  trees::AVLTree<int, helpers::Test> tree{_bst};
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

TEST_F(ContainerTest, BSTMin) { EXPECT_EQ(_bst.min()->num(), -24); }

TEST_F(ContainerTest, BSTMax) { EXPECT_EQ(_bst.max()->num(), 64); }

TEST_F(ContainerTest, BSTHeight) { EXPECT_EQ(_bst.height(), 3); }

TEST_F(ContainerTest, BST_walk_breadth_first) {
  std::array treeAsArr{1, -24, 25, 0, 12, 64, -12};
  std::size_t i{};
  _bst.walk_breadth_first(
      [&treeAsArr, &i](const int, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
}

TEST_F(ContainerTest, BST_walk_depth_first_preorder) {
  std::array treeAsArr{1, -24, 0, -12, 25, 12, 64};
  std::size_t i{};
  _bst.walk_depth_first_preorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
}

TEST_F(ContainerTest, BST_walk_depth_first_inorder) {
  std::array treeAsArr{-24, -12, 0, 1, 12, 25, 64};
  std::size_t i{};
  _bst.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
}

TEST_F(ContainerTest, BST_walk_depth_first_postorder) {
  std::array treeAsArr{-12, 0, -24, 12, 64, 25, 1};
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

  EXPECT_EQ(_bst.is_binary_search_tree(), false);
}

TEST_F(ContainerTest, BST_remove) {
  helpers::Test target{64};
  _bst.remove(64);
  EXPECT_EQ(_bst.max()->num(), 25);
  EXPECT_EQ(_bst.is_binary_search_tree(), true);
}

TEST_F(ContainerTest, BST_inorder_successor_predecessor) {
  helpers::Test target{12};
  EXPECT_EQ(_bst.get_next_inorder(12)->num(), 25);
  EXPECT_EQ(_bst.get_previous_inorder(12)->num(), 1);
}