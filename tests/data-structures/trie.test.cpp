#include <algorithm.hpp>
#include <array>
#include <gtest/gtest.h>
#include <helpers.hpp>
#include <random.hpp>
#include <string>
#include <string_view>
#include <tree.hpp>
#include <vector.hpp>

using namespace std::string_literals;

class ContainerTest : public ::testing::Test {
public:
  trees::GeneralTrie<> _trie{};

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
    _trie.push("hello"s);
    std::cout << _trie;
  };
};

class ContainerTestDelete : public ::testing::Test {
public:
  trees::GeneralTrie<> _trie{};

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
    _trie.push("abcdef"s);
    _trie.push("aa"s);
    _trie.push("ab"s);
    _trie.push("abbcdef"s);
    _trie.push("abbcddef"s);
    _trie.push("acdef"s);
    std::cout << _trie;
  };
};

TEST_F(ContainerTest, Copy) {
  trees::GeneralTrie<> trie{_trie};
  EXPECT_EQ(trie.search("hello"s), true);
  std::cout << trie;
}

TEST_F(ContainerTest, Search) { EXPECT_EQ(_trie.search("hello"s), true); }

TEST_F(ContainerTest, Insert) {
  _trie.push("hello kitty"s);
  _trie.push("hello "s);
  _trie.push("hello ki"s);
  _trie.push("hello k"s);
  _trie.push("hello kitt"s);
  EXPECT_EQ(_trie.search("hello"s), true);
  EXPECT_EQ(_trie.search("hello "s), true);
  EXPECT_EQ(_trie.search("hello ki"s), true);
  EXPECT_EQ(_trie.search("hello k"s), true);
  EXPECT_EQ(_trie.search("hello kitt"s), true);
  EXPECT_EQ(_trie.search("hello kitty"s), true);
  std::cout << _trie;
}

TEST_F(ContainerTest, DeleteRandom) {
  _trie.push("hello kitty"s);
  _trie.push("hello "s);
  _trie.push("hello ki"s);
  _trie.push("hello k"s);
  _trie.push("hello kitt"s);

  _trie.remove("hello "s);
  EXPECT_EQ(_trie.search("hello "s), false);

  _trie.remove("hello ki"s);
  EXPECT_EQ(_trie.search("hello ki"s), false);

  _trie.remove("hello k"s);
  EXPECT_EQ(_trie.search("hello k"s), false);

  _trie.remove("hello kitt"s);
  EXPECT_EQ(_trie.search("hello kitt"s), false);

  _trie.remove("hello kitty"s);
  EXPECT_EQ(_trie.search("hello kitty"s), false);
  std::cout << _trie;

  _trie.remove("hello"s);
  EXPECT_EQ(_trie.search("hello"s), false);
  std::cout << _trie;

  _trie.push("h"s);
  EXPECT_EQ(_trie.search("hello"s), false);
  EXPECT_EQ(_trie.search("h"s), true);
}

TEST_F(ContainerTestDelete, SubtreeHasNoCommonPrefix) {
  EXPECT_EQ(_trie.search("acdef"s), true);
  _trie.remove("acdef"s);
  EXPECT_EQ(_trie.search("acdef"s), false);

  // make sure root a is still there
  EXPECT_EQ(_trie.search("aa"s), true);
  std::cout << _trie;
}

TEST_F(ContainerTestDelete, SubtreeHasACommonPrefix) {
  EXPECT_EQ(_trie.search("abcdef"s), true);
  _trie.remove("abcdef"s);
  EXPECT_EQ(_trie.search("abcdef"s), false);

  // make sure root b is still there
  EXPECT_EQ(_trie.search("ab"s), true);
  EXPECT_EQ(_trie.search("abbcdef"s), true);
  std::cout << _trie;
}

TEST_F(ContainerTestDelete, SubtreeIsAPrefixOfAnotherTree) {
  EXPECT_EQ(_trie.search("ab"s), true);
  _trie.remove("ab"s);
  EXPECT_EQ(_trie.search("ab"s), false);

  // make sure we can delete root b
  _trie.remove("abbcdef"s);
  EXPECT_EQ(_trie.search("abbcdef"s), false);

  EXPECT_EQ(_trie.search("abbcddef"s), true);

  _trie.remove("abbcddef"s);
  EXPECT_EQ(_trie.search("abbcddef"s), false);

  EXPECT_EQ(_trie.search("abcdef"s), true);
  _trie.remove("abcdef"s);
  EXPECT_EQ(_trie.search("abcdef"s), false);
  std::cout << _trie;
}

TEST_F(ContainerTestDelete, PreorderIterative) {
  Vector<char> vec{'\0', 'a', 'a', 'b', 'b', 'c', 'd', 'd', 'e', 'f',
                   'e',  'f', 'c', 'd', 'e', 'f', 'c', 'd', 'e', 'f'};
  std::size_t index{};
  _trie.walk_preorder_iterative([&vec, &index](char letter) {
    EXPECT_EQ(vec.at(index), letter);
    ++index;
  });
  EXPECT_EQ(index, vec.size());
}

TEST_F(ContainerTestDelete, InorderIterative) {
  Vector<char> vec{'a', 'f', 'e', 'd', 'd', 'f', 'e', 'c', 'b', 'b',
                   'f', 'e', 'd', 'c', 'a', 'f', 'e', 'd', 'c', '\0'};
  std::size_t index{};
  _trie.walk_inorder_iterative([&vec, &index](char letter) {
    EXPECT_EQ(vec.at(index), letter);
    ++index;
  });
  EXPECT_EQ(index, vec.size());
}

TEST_F(ContainerTestDelete, PostorderIterative) {
  Vector<char> vec{'a', 'f', 'e', 'd', 'f', 'e', 'd', 'c', 'b', 'f',
                   'e', 'd', 'c', 'b', 'f', 'e', 'd', 'c', 'a', '\0'};
  std::size_t index{};
  _trie.walk_postorder_iterative([&vec, &index](char letter) {
    EXPECT_EQ(vec.at(index), letter);
    ++index;
  });
  EXPECT_EQ(index, vec.size());
}

TEST_F(ContainerTestDelete, GetAllStr) {
  Vector<std::string> vec{"aa"s,      "ab"s,     "abbcddef"s,
                          "abbcdef"s, "abcdef"s, "acdef"s};
  Vector<std::string> vec1{_trie.get_all_str()};
  for (std::size_t i{}; i < vec.size(); ++i) {
    EXPECT_EQ(vec[i], vec1[i]);
  }
}

TEST_F(ContainerTest, AutoComplete) {
  _trie.push("hello there"s);
  _trie.push("hello Micheal"s);
  _trie.push("hello Michalel"s);
  _trie.push("hello Jeff"s);
  _trie.push("morning sun"s);
  _trie.push("hellokaka"s);
  std::string prefix{"hello"s};
  Vector<std::string> vec{prefix + ""s, prefix + " Jeff"s,
                          prefix + " Michalel"s};
  Vector<std::string> vec1{_trie.autocomplete(prefix, 3)};
  for (std::size_t i{}; i < vec.size(); ++i) {
    EXPECT_EQ(prefix + vec1[i], vec[i]);
  }

  EXPECT_EQ(_trie.autocomplete("hdello Micheal").size(), 0);

  EXPECT_EQ(_trie.autocomplete("ballo").size(), 0);
}