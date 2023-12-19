#pragma once

#include <algorithm>
#include <allocator.hpp>
#include <concept.hpp>
#include <functional>
#include <helpers.hpp>
#include <queue.hpp>
#include <utility>

#define AVL_TREE_DEBUG 0

#if AVL_TREE_DEBUG == 1
#define AVL_TREE_DEBUG_MS(mes)                                                 \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define AVL_TREE_DEBUG_MS(mes)                                                 \
  do {                                                                         \
  } while (0)
#endif

namespace trees {
template <concepts::Comparable T, typename Data,
          concepts::Allocator Allocator = Allocator<T>>
class AVLTree {
private:
  class Node;

public:
  using key_type = T;
  using value_type = Data;
  using pointer = Data*;
  using reference = Data&;
  using rvalue_reference = Data&&;
  using const_reference = const Data&;
  using self = AVLTree<T, Data, Allocator>;

  AVLTree() = default;
  ~AVLTree() {
    _walk_postorder(_root, [](Node* node) { delete node; });
  };

  AVLTree(self& other) {
    other.walk_depth_first_preorder(
        [this](const key_type& key, reference ele) { push(key, ele); });
  };
  AVLTree(self&& other) : _root{other._root} { other._root = nullptr; }

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

  template <typename Key, typename Value> void push(Key&& key, Value&& value) {
    _root = _push(_root, std::forward(key), std::forward(value));
  }

  void remove(const key_type& key) { _root = _delete(_root, key); };

  pointer search(const key_type& key) {
    Node* result = _search(_root, key);
    return result ? &(result->_value) : nullptr;
  };

  pointer min() {
    Node* result{_min(_root)};
    return result ? &(result->_value) : nullptr;
  };

  pointer max() {
    Node* result{_max(_root)};
    return result ? &(result->_value) : nullptr;
  };

  pointer get_next_inorder(const key_type& key) {
    Node* node{_inorder_successor(_root, key)};
    return node ? &(node->_value) : nullptr;
  }

  pointer get_previous_inorder(const key_type& key) {
    Node* node{_inorder_predecessor(_root, key)};
    return node ? &(node->_value) : nullptr;
  }

  int height() { return _root ? _root->height() : 0; }

  void walk_breadth_first(std::function<void(const key_type&, reference)> fn) {
    if (!_root) {
      return;
    }
    Queue<Node*> queue(5);
    queue.push_back(_root);
    while (!queue.empty()) {
      Node* current{queue.pop_front()};
      fn(current->_key, current->_value);
      if (current->_left) {
        queue.push_back(current->_left);
      }
      if (current->_right) {
        queue.push_back(current->_right);
      }
    }
  }

  void walk_depth_first_preorder(
      std::function<void(const key_type&, reference)> fn) {
    _walk_preorder(_root, [&fn](Node* node) { fn(node->_key, node->_value); });
  }

  void
  walk_depth_first_inorder(std::function<void(const key_type&, reference)> fn) {
    _walk_inorder(_root, [&fn](Node* node) { fn(node->_key, node->_value); });
  }

  void walk_depth_first_postorder(
      std::function<void(const key_type&, reference)> fn) {
    _walk_postorder(_root, [&fn](Node* node) { fn(node->_key, node->_value); });
  }

  bool is_binary_search_tree() { return _is_bst(_root, nullptr, nullptr); }

  void swap(self& other) noexcept {
    using std::swap;
    swap(_root, other._root);
  }
  friend void swap(self& e1, self& e2) { e1.swap(e2); };

private:
  class Node {
    friend class AVLTree;

    int _height{};
    Node* _left{nullptr};
    Node* _right{nullptr};
    key_type _key{};
    value_type _value{};

  public:
    Node() = default;

    template <typename Key, typename Value>
    Node(Key&& key, Value&& value)
        : _key{std::forward(key)}, _value{std::forward(value)} {}

    ~Node() = default;

    Node(const Node& other) = default;
    Node& operator=(const Node& other) = default;

    Node(Node&& other) = default;
    Node& operator=(Node&& other) = default;

    int _get_balance_factor() {
      int nodeLeftHeight = this->_left ? this->_left->_height : -1;
      int nodeRightHeight = this->_right ? this->_right->_height : -1;
      return nodeLeftHeight - nodeRightHeight;
    }

    bool _is_subtree_balanced() {
      int balanceFactor{_get_balance_factor()};
      return balanceFactor >= -1 && balanceFactor <= 1;
    }

    int height() { return this->_height; }
  };

  Node* _root{nullptr};

  Node* _delete(Node* node, const key_type& key) {
    if (!node) {
      return nullptr;
    } else if (node->_key > key) {
      node->_left = _delete(node->_left, key);
    } else if (node->_key < key) {
      node->_right = _delete(node->_right, key);
    } else {
      // found ele, delete
      if (!node->_left && !node->_right) {
        // case 1: no child
        delete node;
        node = nullptr;
      } else if (!node->_left) {
        // case 2: 1 child
        Node* tmp{node};
        node = node->_right;
        delete tmp;
      } else if (!node->_right) {
        Node* tmp{node};
        node = node->_left;
        delete tmp;
      } else {
        // case 3: 2 children
        Node* maxLeftNode{_max(node->_left)};
        std::swap(node->_value, maxLeftNode->_value);
        std::swap(node->_key, maxLeftNode->_key);
        node->_left = _delete(node->_left, maxLeftNode->_key);
      }
    }
    return node;
  }

  bool _is_bst(Node* node, key_type* minEle, key_type* maxEle) {
    if (!node) {
      return true;
    }
    bool isGreaterThanMin(minEle ? node->_key > *minEle : true);
    bool isLessThanMax(maxEle ? node->_key < *maxEle : true);
    return isGreaterThanMin && isLessThanMax &&
           _is_bst(node->_left, minEle, &(node->_key)) &&
           _is_bst(node->_right, &(node->_key), maxEle);
  }

  Node* _min(Node* node) {
    if (!node) {
      return nullptr;
    }
    while (node->_left) {
      node = node->_left;
    }
    return node;
  }

  Node* _max(Node* node) {
    if (!node) {
      return nullptr;
    }
    while (node->_right) {
      node = node->_right;
    }
    return node;
  }

  // O(h)
  Node* _inorder_successor(Node* node, const key_type& key) {
    Node* current{_search(node, key)};
    if (!current) {
      return nullptr;
    }
    // if there is right subtree, find the min
    if (current->_right) {
      return _min(current->_right);
    } else {
      // no right subtree, need to walk to nearest ancestor that the current
      // node is on the left
      Node* ancestor{_root};
      Node* successor{nullptr};
      while (ancestor != current) {
        if (current->_key < ancestor->_key) {
          successor = ancestor;
          ancestor = ancestor->_left;
        } else {
          ancestor = ancestor->_right;
        }
      }
      return successor;
    }
  }

  // O(h)
  Node* _inorder_predecessor(Node* node, const key_type& key) {
    Node* current{_search(node, key)};
    if (!current) {
      return nullptr;
    }
    // if there is left subtree, find the max
    if (current->_left) {
      return _max(current->_left);
    } else {
      // no left subtree, need to walk to nearest ancestor that the current
      // node is on the right
      Node* ancestor{_root};
      Node* successor{nullptr};
      while (ancestor != current) {
        if (current->_key > ancestor->_key) {
          successor = ancestor;
          ancestor = ancestor->_right;
        } else {
          ancestor = ancestor->_left;
        }
      }
      return successor;
    }
  }

  void _walk_preorder(Node* node, const std::function<void(Node*)>& fn) {
    if (!node) {
      return;
    }
    fn(node);
    _walk_preorder(node->_left, fn);
    _walk_preorder(node->_right, fn);
  }

  void _walk_inorder(Node* node, const std::function<void(Node*)>& fn) {
    if (!node) {
      return;
    }
    _walk_inorder(node->_left, fn);
    fn(node);
    _walk_inorder(node->_right, fn);
  }

  void _walk_postorder(Node* node, const std::function<void(Node*)>& fn) {
    if (!node) {
      return;
    }
    _walk_postorder(node->_left, fn);
    _walk_postorder(node->_right, fn);
    fn(node);
  }

  Node* _search(Node* node, const key_type& key) {
    if (!node || node->_key == key) {
      return node;
    } else if (node->_key > key) {
      return _search(node->_left, key);
    } else {
      return _search(node->_right, key);
    }
  }

  template <typename Key, typename Value>
  Node* _push(Node* node, Key&& key, Value&& value) {
    if (!node) {
      return new Node{std::forward(key), std::forward(value)};
    } else if (node->_key < key) {
      node->_right =
          _push(node->_right, std::forward(key), std::forward(value));
    } else if (node->_key > key) {
      node->_left = _push(node->_left, std::forward(key), std::forward(value));
    }
    return node;
  }
};
} // namespace trees