#pragma once

#include <algorithm>
#include <allocator.hpp>
#include <concept.hpp>
#include <functional>
#include <helpers.hpp>
#include <iostream>
#include <map>
#include <queue.hpp>
#include <stack.hpp>
#include <string>
#include <utility>

#define TRIE_TREE_DEBUG 0

#if TRIE_TREE_DEBUG == 1
#define TRIE_TREE_DEBUG_MS(mes)                                                \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define TRIE_TREE_DEBUG_MS(mes)                                                \
  do {                                                                         \
  } while (0)
#endif

namespace trees {
template <concepts::Comparable CharType = char,
          typename Allocator = Allocator<void>>
class GeneralTrie {
private:
  class Node;

public:
  using key_type = CharType;
  using value_type = std::basic_string<key_type>;
  using pointer = value_type*;
  using reference = value_type&;
  using rvalue_reference = value_type&&;
  using const_reference = const value_type&;
  using self = GeneralTrie<key_type, Allocator>;

  GeneralTrie() = default;
  ~GeneralTrie() {
    _walk_postorder(_root, [](Node* node) { delete node; });
  };

  GeneralTrie(self& other) : _root{_copy_subtree(other._root)} {};
  GeneralTrie(self&& other) : _root{other._root} { other._root = nullptr; }

  self& operator=(const self& other) {
    self tmp{other};
    std::swap(tmp, *this);
    return *this;
  }

  self& operator=(self&& other) {
    self tmp{std::move(other)};
    std::swap(tmp, *this);
    return *this;
  }

  template <concepts::IsSameBase<value_type> String> void push(String&& str) {
    if (!_root) {
      _root = new Node();
    }
    Node* node{_root};
    for (auto& i : str) {
      auto iter{node->_children.find(i)};
      if (iter == node->_children.end()) {
        std::tie(iter, std::ignore) = node->_children.insert({i, new Node()});
      }
      node = iter->second;
    }
    node->_isEndOfWord = true;
  }

  // we remove an entire subtree, but if the str has a common prefix then we
  // stop at the nearest common ancestor
  template <concepts::IsSameBase<value_type> String> void remove(String&& str) {
    if (!_root) {
      return;
    }
    Stack<Node*> stack{};
    Node* node{_root};
    Node* lastCommonAncestor{nullptr};
    CharType lastCommonAncestorKey{};
    for (auto& i : str) {
      auto iter{node->_children.find(i)};
      if (iter == node->_children.end()) {
        return;
      }
      if (node->_children.size() == 1 && !node->_isEndOfWord) {
        stack.push(node);
      } else {
        while (stack.size()) {
          stack.pop();
        }
        lastCommonAncestor = node;
        lastCommonAncestorKey = i;
      }
      node = iter->second;
    }
    if (node->_isEndOfWord) {
      if (node->_can_delete()) {
        delete node;
        while (stack.size()) {
          Node* parent{stack.pop()};
          if (parent == _root) {
            _root = nullptr;
          }
          delete parent;
        }
        if (lastCommonAncestor) {
          lastCommonAncestor->_children.erase(lastCommonAncestorKey);
        }
      } else {
        node->_isEndOfWord = false;
      }
    }
    return;
  }

  template <concepts::IsSameBase<value_type> String> bool search(String&& str) {
    if (!_root) {
      return false;
    }
    Node* node{_root};
    for (auto& i : str) {
      auto iter{node->_children.find(i)};
      if (iter == node->_children.end()) {
        return false;
      }
      node = iter->second;
    }
    return node->_isEndOfWord;
  }

  void swap(self& other) noexcept {
    using std::swap;
    swap(_root, other._root);
  }
  friend void swap(self& e1, self& e2) { e1.swap(e2); }

  friend std::ostream& operator<<(std::ostream& os, const self& tree) {
    if (!tree._root) {
      return os;
    }
    Queue<Node*> queue{};
    int rowNodeCount{1};
    int nextRowNodeCount{};
    queue.push_back(tree._root);
    while (!queue.empty()) {
      Node* node{queue.pop_front()};
      for (auto& i : node->_children) {
        os << "[" << i.first << "]  ";
        if (!node->is_leaf()) {
          queue.push_back(i.second);
          nextRowNodeCount += 1;
        }
      }

      --rowNodeCount;

      if (rowNodeCount == 0) {
        os << "\n";
        rowNodeCount = nextRowNodeCount;
        nextRowNodeCount = 0;
      }
    }
    return os;
  }

private:
  class Node {
    friend class GeneralTrie;

    std::map<key_type, Node*> _children{};
    bool _isEndOfWord{false};

  public:
    Node() = default;

    ~Node() {}

    Node(const Node& other) = delete;
    Node& operator=(const Node& other) = delete;

    Node(Node&& other) = default;
    Node& operator=(Node&& other) = default;

    bool is_leaf() { return _children.size() == 0; }

    void* operator new(std::size_t size) {
      typename Allocator::rebind<Node>::other alloc{};
      return static_cast<void*>(alloc.allocate(size));
    }

    void operator delete(void* p, std::size_t) {
      std::cout << "NODE DESTRUCTOR"
                << "\n";
      typename Allocator::rebind<Node>::other alloc{};
      alloc.deallocate(static_cast<Node*>(p));
      return;
    }

    bool _can_delete() { return _children.size() == 0 && _isEndOfWord; }
  };

  Node* _root{nullptr};

  void _walk_preorder(Node* node, const std::function<void(Node*)>& fn) {
    if (!node) {
      return;
    }
    fn(node);
    for (auto& i : node->_children) {
      _walk_preorder(i.second, fn);
    }
  }

  void _walk_inorder(Node* node, const std::function<void(Node*)>& fn) {
    if (!node) {
      return;
    }
    auto lastElement{node->_children.end() - 1};
    for (auto i{node->_children.begin()}; i != lastElement; ++i) {
      _walk_inorder(i->second, fn);
    }
    fn(node);
    _walk_inorder(lastElement->second);
  }

  void _walk_postorder(Node* node, const std::function<void(Node*)>& fn) {
    if (!node) {
      return;
    }
    for (auto& i : node->_children) {
      _walk_postorder(i.second, fn);
    }
    fn(node);
  }

  Node* _copy_subtree(Node* node) {
    Node* newNode{new Node{}};
    newNode->_isEndOfWord = node->_isEndOfWord;
    if (!node->is_leaf()) {
      for (auto& i : node->_children) {
        newNode->_children.insert({i.first, _copy_subtree(i.second)});
      }
    }

    return newNode;
  }
};
} // namespace trees