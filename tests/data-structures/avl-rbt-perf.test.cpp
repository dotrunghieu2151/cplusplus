#include <gtest/gtest.h>
#include <helpers.hpp>
#include <iostream>
#include <timer.hpp>
#include <tree.hpp>

TEST(PerfTest, Insertion) {
  trees::AVLTree<int, helpers::Test> avl;
  trees::RBT<int, helpers::Test> rbt;
  Timer timer{};
  for (int i{}; i < 1000000; ++i) {
    avl.push(i, helpers::Test{i});
  }
  double avlInsertionTime{timer.elapsed()};
  std::cout << "AVL INSERTION TIME: " << avlInsertionTime << "\n";
  timer.reset();

  for (int i{}; i < 1000000; ++i) {
    rbt.push(i, helpers::Test{i});
  }
  double rbtInsertionTime{timer.elapsed()};
  std::cout << "RBT INSERTION TIME: " << rbtInsertionTime << "\n";

  EXPECT_GT(avlInsertionTime, rbtInsertionTime);
}

TEST(PerfTest, Search) {
  trees::AVLTree<int, helpers::Test> avl;
  trees::RBT<int, helpers::Test> rbt;
  for (int i{}; i < 1000000; ++i) {
    avl.push(i, helpers::Test{i});
    rbt.push(i, helpers::Test{i});
  }

  Timer timer{};

  helpers::Test* a{avl.search(647)};
  double avlSearchTime{timer.elapsed()};
  std::cout << "AVL SEARCH TIME: " << avlSearchTime << " VALUE: " << a->num()
            << "\n";

  timer.reset();

  helpers::Test* b{rbt.search(647)};
  double rbtSearchTime{timer.elapsed()};

  std::cout << "RBT SEARCH TIME: " << rbtSearchTime << " VALUE: " << b->num()
            << "\n";

  EXPECT_GT(rbtSearchTime, avlSearchTime);
}

TEST(PerfTest, Delete) {
  trees::AVLTree<int, helpers::Test> avl;
  trees::RBT<int, helpers::Test> rbt;
  for (int i{}; i < 1000000; ++i) {
    avl.push(i, helpers::Test{i});
    rbt.push(i, helpers::Test{i});
  }

  Timer timer{};

  avl.remove(647);
  double avlDeleteTime{timer.elapsed()};
  std::cout << "AVL Delete TIME: " << avlDeleteTime << "\n";

  timer.reset();

  rbt.remove(647);
  double rbtDeleteTime{timer.elapsed()};

  std::cout << "RBT Delete TIME: " << rbtDeleteTime << "\n";

  EXPECT_GT(avlDeleteTime, rbtDeleteTime);
}