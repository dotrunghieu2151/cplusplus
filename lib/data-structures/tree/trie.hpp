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
#include <vector.hpp>

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
    _walk_postorder(nullptr, _root,
                    [](const CharType*, Node* node) { delete node; });
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
  void remove(const value_type& str) {
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

  bool search(const value_type& str) const { return _search_node(_root, str); }
  Vector<value_type> autocomplete(const value_type& str) const {
    if (!_root) {
      return {};
    }
    Node* lastPrefixNode{_search_node(_root, str)};
    if (!lastPrefixNode) {
      return {};
    }
    Vector<value_type> result{};
    if (lastPrefixNode->_isEndOfWord) {
      result.push_back("");
    }
    if (!lastPrefixNode->is_leaf()) {
      value_type prefix{};
      _get_str(lastPrefixNode, prefix, result);
    }
    return result;
  }

  Vector<value_type> autocomplete(const value_type& str,
                                  unsigned int max) const {
    if (!_root) {
      return {};
    }
    Node* lastPrefixNode{_search_node(_root, str)};
    if (!lastPrefixNode) {
      return {};
    }
    Vector<value_type> result{};
    if (lastPrefixNode->_isEndOfWord) {
      result.push_back("");
    }
    if (!lastPrefixNode->is_leaf()) {
      value_type prefix{};
      _get_str(lastPrefixNode, prefix, result, max);
    }
    return result;
  }

  void walk_preorder_iterative(const std::function<void(const CharType)> fn) {
    if (!_root) {
      return;
    }
    _walk_preorder_iterative(_root, [&fn](const CharType* letter, Node*) {
      fn(letter ? *letter : '\0');
    });
  }

  void walk_inorder_iterative(const std::function<void(const CharType)> fn) {
    if (!_root) {
      return;
    }
    _walk_inorder_iterative(_root, [&fn](const CharType* letter, Node*) {
      fn(letter ? *letter : '\0');
    });
  }

  void walk_postorder_iterative(const std::function<void(const CharType)> fn) {
    if (!_root) {
      return;
    }
    _walk_postorder_iterative(_root, [&fn](const CharType* letter, Node*) {
      fn(letter ? *letter : '\0');
    });
  }

  Vector<value_type> get_all_str() const {
    Vector<value_type> result{};
    value_type prefix{};
    _get_str(_root, prefix, result);
    return result;
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

  Node* _search_node(Node* node, const value_type& str) const {
    if (!node) {
      return nullptr;
    }
    for (auto& i : str) {
      auto iter{node->_children.find(i)};
      if (iter == node->_children.end()) {
        return nullptr;
      }
      node = iter->second;
    }
    return node->_isEndOfWord ? node : nullptr;
  }

  Node* _get_longest_common_prefix_node(Node* root,
                                        const value_type& str) const {
    if (!root) {
      return nullptr;
    }
    Node* node{root};
    for (auto& i : str) {
      auto iter{node->_children.find(i)};
      if (iter == node->_children.end()) {
        break;
      }
      node = iter->second;
    }
    return node == root ? nullptr : node;
  }

  void _get_str(Node* root, const value_type& prefix,
                Vector<value_type>& result) const {
    if (!root) {
      return;
    }
    for (auto& i : root->_children) {
      value_type newPrefix{prefix + i.first};
      if (i.second->_isEndOfWord) {
        result.push_back(newPrefix);
      }
      if (!i.second->is_leaf()) {
        _get_str(i.second, newPrefix, result);
      }
    }
    return;
  }

  void _get_str(Node* root, const value_type& prefix,
                Vector<value_type>& result, unsigned int max) const {
    if (result.size() == max) {
      return;
    }
    if (!root) {
      return;
    }
    for (auto& i : root->_children) {
      value_type newPrefix{prefix + i.first};
      if (i.second->_isEndOfWord) {
        result.push_back(newPrefix);
      }
      if (!i.second->is_leaf()) {
        _get_str(i.second, newPrefix, result, max);
      }
    }
    return;
  }

  void _walk_preorder(const CharType* letter, Node* node,
                      const std::function<void(const CharType*, Node*)>& fn) {
    if (!node) {
      return;
    }
    fn(letter, node);
    for (auto& i : node->_children) {
      _walk_preorder(&i.first, i.second, fn);
    }
  }

  void _walk_preorder_iterative(
      Node* node, const std::function<void(const CharType*, Node*)>& fn) {
    if (!node) {
      return;
    }
    Stack<std::pair<const CharType*, Node*>,
          Vector<std::pair<const CharType*, Node*>>>
        stack{};
    const CharType* currentChar{nullptr};
    stack.push({currentChar, node});

    while (!stack.empty()) {
      std::tie(currentChar, node) = stack.pop();
      fn(currentChar, node);
      if (!node->is_leaf()) {
        for (auto i{node->_children.rbegin()}; i != node->_children.rend();
             ++i) {
          // const CharType* key{};
          stack.push({&i->first, i->second});
        }
      }
    }
  }

  void _walk_inorder_iterative(
      Node* node, const std::function<void(const CharType*, Node*)>& fn) {
    if (!node) {
      return;
    }
    Stack<std::pair<const CharType*, Node*>,
          Vector<std::pair<const CharType*, Node*>>>
        stack{};
    Node* currentNode{node};
    const CharType* currentChar{nullptr};
    while (!stack.empty() || currentNode) {
      if (!currentNode) {
        // signifies end of subtree
        auto next{stack.pop()};
        fn(next.first, next.second);
        if (next.second->is_leaf()) {
          currentNode = nullptr;
          currentChar = nullptr;
        } else {
          if (next.second->_children.size() > 1) {
            auto iter{next.second->_children.rbegin()};
            currentNode = iter->second;
            currentChar = &iter->first;
          }
        }
      } else if (currentNode->is_leaf()) {
        fn(currentChar, currentNode);
        auto next{stack.top()};
        if (!next.second->is_leaf() &&
            !next.second->_children.contains(*currentChar)) {
          std::tie(currentChar, currentNode) = stack.pop();
        } else {
          currentNode = nullptr;
          currentChar = nullptr;
        }
      } else {
        stack.push({currentChar, currentNode});
        if (currentNode->_children.size() > 1) {
          for (auto i{++currentNode->_children.rbegin()};
               i != (--currentNode->_children.rend()); ++i) {
            stack.push({&i->first, i->second});
          }
        }
        auto firstIter{currentNode->_children.begin()};
        currentNode = firstIter->second;
        currentChar = &firstIter->first;
      }
    }
  }

  void _walk_inorder(const CharType* letter, Node* node,
                     const std::function<void(const CharType*, Node*)>& fn) {
    if (!node) {
      return;
    }
    auto lastElement{node->_children.end() - 1};
    for (auto i{node->_children.begin()}; i != lastElement; ++i) {
      _walk_inorder(&i->first, i->second, fn);
    }
    fn(letter, node);
    _walk_inorder(&lastElement->first, lastElement->second);
  }

  void _walk_postorder(const CharType* letter, Node* node,
                       const std::function<void(const CharType*, Node*)>& fn) {
    if (!node) {
      return;
    }
    for (auto& i : node->_children) {
      _walk_postorder(&i.first, i.second, fn);
    }
    fn(letter, node);
  }

  void _walk_postorder_iterative(
      Node* node, const std::function<void(const CharType*, Node*)>& fn) {
    if (!node) {
      return;
    }
    Stack<std::pair<const CharType*, Node*>,
          Vector<std::pair<const CharType*, Node*>>>
        stack{};
    Node* currentNode{node};
    const CharType* currentChar{nullptr};
    while (stack.size() || currentNode) {
      if (!currentNode) {
        std::tie(currentChar, currentNode) = stack.pop();
        fn(currentChar, currentNode);
        if (!currentNode->is_leaf() && stack.size() &&
            !stack.top().second->_children.contains(*currentChar)) {
          std::tie(currentChar, currentNode) = stack.pop();
        } else {
          currentChar = nullptr;
          currentNode = nullptr;
        }
      } else if (currentNode->is_leaf()) {
        fn(currentChar, currentNode);
        auto next{stack.top()};
        if (!next.second->is_leaf() &&
            !next.second->_children.contains(*currentChar)) {
          std::tie(currentChar, currentNode) = stack.pop();
        } else {
          currentChar = nullptr;
          currentNode = nullptr;
        }
      } else {
        stack.push({currentChar, currentNode});
        if (currentNode->_children.size() > 1) {
          for (auto i{currentNode->_children.rbegin()};
               i != --currentNode->_children.rend(); ++i) {
            stack.push({&i->first, i->second});
          }
        }
        auto firstChild{currentNode->_children.begin()};
        currentNode = firstChild->second;
        currentChar = &firstChild->first;
      }
    }
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