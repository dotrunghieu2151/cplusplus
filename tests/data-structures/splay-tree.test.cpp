#include <array>
#include <gtest/gtest.h>
#include <helpers.hpp>
#include <iostream>
#include <random.hpp>
#include <string>
#include <string_view>
#include <tree.hpp>

class ContainerTest : public ::testing::Test {
public:
  trees::SplayTree<int, helpers::Test> _bst{};

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

    //           [-12]
    //    [-24]       [25]
    //              [0]  [64]
    //                [1]
    //                  [12]
    std::cout << _bst;
  };
};

TEST_F(ContainerTest, BSTCopy) {
  trees::SplayTree<int, helpers::Test> tree{_bst};
  EXPECT_EQ(tree.is_binary_search_tree(), true);
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

TEST_F(ContainerTest, BSTHeight) { EXPECT_EQ(_bst.height(), 4); }

TEST_F(ContainerTest, BST_walk_breadth_first) {
  std::array treeAsArr{-12, -24, 25, 0, 64, 1, 12};
  std::size_t i{};
  _bst.walk_breadth_first(
      [&treeAsArr, &i](const int, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
}

TEST_F(ContainerTest, BST_walk_depth_first_preorder) {
  std::array treeAsArr{-12, -24, 25, 0, 1, 12, 64};
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
  std::array treeAsArr{-24, 12, 1, 0, 64, 25, -12};
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
  trees::SplayTree<int, helpers::Test> avlTree{};
  avlTree.push(1, helpers::Test{1});
  avlTree.push(24, helpers::Test{24});
  avlTree.push(25, helpers::Test{25});
  avlTree.push(12, helpers::Test{12});
  avlTree.push(0, helpers::Test{0});
  avlTree.push(64, helpers::Test{64});
  avlTree.push(30, helpers::Test{30});
  avlTree.push(45, helpers::Test{45});
  avlTree.push(50, helpers::Test{50});
  Vector<int> treeAsArr{0, 1, 12, 24, 25, 30, 45, 50};
  std::size_t i{};
  avlTree.remove(64);
  EXPECT_EQ(avlTree.max()->num(), 50);
  auto t{avlTree.search(64)};
  EXPECT_EQ(t, nullptr);

  avlTree.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });

  std::cout << avlTree << "\n";

  treeAsArr = {0, 1, 12, 24, 25, 45, 50};
  i = 0;

  avlTree.remove(30);
  EXPECT_EQ(avlTree.max()->num(), 50);
  EXPECT_EQ(avlTree.search(30), nullptr);

  avlTree.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });

  std::cout << avlTree << "\n";

  treeAsArr = {0, 1, 12, 25, 45, 50};
  i = 0;

  avlTree.remove(24);
  EXPECT_EQ(avlTree.max()->num(), 50);
  EXPECT_EQ(avlTree.search(24), nullptr);

  avlTree.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });

  std::cout << avlTree << "\n";

  avlTree.push(60, helpers::Test{60});
  avlTree.push(70, helpers::Test{70});
  avlTree.push(30, helpers::Test{30});
  avlTree.push(80, helpers::Test{80});

  treeAsArr = {0, 1, 12, 30, 45, 50, 60, 70, 80};
  i = 0;

  avlTree.remove(25);

  avlTree.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });

  std::cout << avlTree << "\n";

  avlTree.remove(80);
  avlTree.remove(70);
  avlTree.remove(60);
  avlTree.remove(50);

  treeAsArr = {0, 1, 12, 30, 45};
  i = 0;
  avlTree.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });

  std::cout << avlTree << "\n";
  avlTree.remove(12);

  treeAsArr = {0, 1, 30, 45};
  i = 0;
  avlTree.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
  avlTree.remove(45);

  treeAsArr = {0, 1, 30};
  i = 0;
  avlTree.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
  avlTree.remove(1);

  treeAsArr = {0, 30};
  i = 0;
  avlTree.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
  avlTree.remove(0);

  treeAsArr = {30};
  i = 0;
  avlTree.walk_depth_first_inorder(
      [&treeAsArr, &i](const int&, helpers::Test& data) -> void {
        EXPECT_EQ(treeAsArr[i], data.num());
        ++i;
      });
  avlTree.remove(30);

  std::cout << avlTree << "\n";
}

TEST_F(ContainerTest, BST_inorder_successor_predecessor) {
  helpers::Test target{12};
  EXPECT_EQ(_bst.get_next_inorder(25)->num(), 64);
  EXPECT_EQ(_bst.get_previous_inorder(25)->num(), 12);
}