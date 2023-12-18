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
  struct Node;

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
  AVLTree(self&& other) : _root{other._root} { other._root = nullptr; };

  self& operator=(const self& other) {
    self tmp{other};
    std::swap(tmp, *this);
    return *this;
  };

  self& operator=(self&& other) {
    self tmp{std::move(other)};
    std::swap(tmp, *this);
    return *this;
  };

  template <std::same_as<key_type> Key, std::same_as<value_type> Value>
  void push(Key&& key, Value&& value) {
    _root = _push(_root, std::forward(key), std::forward(value));
  };

  void remove(const key_type& key) { _root = _delete(_root, key); };

  pointer search(const key_type& key) {
    Node* result = _search(_root, key);
    return result ? &(result->value) : nullptr;
  };

  pointer min() {
    Node* result{_min(_root)};
    return result ? &(result->value) : nullptr;
  };

  pointer max() {
    Node* result{_max(_root)};
    return result ? &(result->value) : nullptr;
  };

  pointer get_next_inorder(const key_type& key) {
    Node* node{_inorder_successor(_root, key)};
    return node ? &(node->value) : nullptr;
  }

  pointer get_previous_inorder(const key_type& key) {
    Node* node{_inorder_predecessor(_root, key)};
    return node ? &(node->value) : nullptr;
  }

  int height() { return _height(_root); }

  void walk_breadth_first(std::function<void(const key_type&, reference)> fn) {
    if (!_root) {
      return;
    }
    Queue<Node*> queue(5);
    queue.push_back(_root);
    while (!queue.empty()) {
      Node* current{queue.pop_front()};
      fn(current->key, current->value);
      if (current->left) {
        queue.push_back(current->left);
      }
      if (current->right) {
        queue.push_back(current->right);
      }
    }
  }

  void walk_depth_first_preorder(
      std::function<void(const key_type&, reference)> fn) {
    _walk_preorder(_root, [&fn](Node* node) { fn(node->key, node->value); });
  }

  void
  walk_depth_first_inorder(std::function<void(const key_type&, reference)> fn) {
    _walk_inorder(_root, [&fn](Node* node) { fn(node->key, node->value); });
  }

  void walk_depth_first_postorder(
      std::function<void(const key_type&, reference)> fn) {
    _walk_postorder(_root, [&fn](Node* node) { fn(node->key, node->value); });
  }

  bool is_binary_search_tree() { return _is_bst(_root, nullptr, nullptr); }

  void swap(self& other) noexcept {
    using std::swap;
    swap(_root, other._root);
  }
  friend void swap(self& e1, self& e2) { e1.swap(e2); };

private:
  struct Node {
    value_type value{};
    key_type key{};
    Node* left{nullptr};
    Node* right{nullptr};
    int height{};
  };

  Node* _root{nullptr};

  Node* _delete(Node* node, const_reference ele) {
    if (!node) {
      return nullptr;
    } else if (node->value > ele) {
      node->left = _delete(node->left, ele);
    } else if (node->value < ele) {
      node->right = _delete(node->right, ele);
    } else {
      // found ele, delete
      if (!node->left && !node->right) {
        // case 1: no child
        delete node;
        node = nullptr;
      } else if (!node->left) {
        // case 2: 1 child
        Node* tmp{node};
        node = node->right;
        delete tmp;
      } else if (!node->right) {
        Node* tmp{node};
        node = node->left;
        delete tmp;
      } else {
        // case 3: 2 children
        Node* maxLeftNode{_max(node->left)};
        std::swap(node->value, maxLeftNode->value);
        node->left = _delete(node->left, maxLeftNode->value);
      }
    }
    return node;
  }

  bool _is_bst(Node* node, pointer minEle, pointer maxEle) {
    if (!node) {
      return true;
    }
    bool isGreaterThanMin(minEle ? node->value > *minEle : true);
    bool isLessThanMax(maxEle ? node->value < *maxEle : true);
    return isGreaterThanMin && isLessThanMax &&
           _is_bst(node->left, minEle, &(node->value)) &&
           _is_bst(node->right, &(node->value), maxEle);
  }

  int _get_balance_factor(Node* node) {
    int nodeLeftHeight = node->left ? node->left->height : -1;
    int nodeRightHeight = node->right ? node->right->height : -1;
    return nodeLeftHeight - nodeRightHeight;
  }

  bool _is_subtree_balanced(Node* node) {
    int balanceFactor{_get_balance_factor(node)};
    return balanceFactor >= -1 && balanceFactor <= 1;
  }

  Node* _min(Node* node) {
    if (!node) {
      return nullptr;
    }
    while (node->left) {
      node = node->left;
    }
    return node;
  }

  Node* _max(Node* node) {
    if (!node) {
      return nullptr;
    }
    while (node->right) {
      node = node->right;
    }
    return node;
  }

  // O(h)
  Node* _inorder_successor(Node* node, const key_type& key) {
    Node* current{_search(ele, node)};
    if (!current) {
      return nullptr;
    }
    // if there is right subtree, find the min
    if (current->right) {
      return _min(current->right);
    } else {
      // no right subtree, need to walk to nearest ancestor that the current
      // node is on the left
      Node* ancestor{_root};
      Node* successor{nullptr};
      while (ancestor != current) {
        if (current->value < ancestor->value) {
          successor = ancestor;
          ancestor = ancestor->left;
        } else {
          ancestor = ancestor->right;
        }
      }
      return successor;
    }
  }

  // O(h)
  Node* _inorder_predecessor(Node* node, const_reference ele) {
    Node* current{_search(ele, node)};
    if (!current) {
      return nullptr;
    }
    // if there is left subtree, find the max
    if (current->left) {
      return _max(current->left);
    } else {
      // no left subtree, need to walk to nearest ancestor that the current
      // node is on the right
      Node* ancestor{_root};
      Node* successor{nullptr};
      while (ancestor != current) {
        if (current->value > ancestor->value) {
          successor = ancestor;
          ancestor = ancestor->right;
        } else {
          ancestor = ancestor->left;
        }
      }
      return successor;
    }
  }

  int _height(Node* node) {
    if (!node) {
      return -1;
    }
    return std::max<int>(_height(node->left), _height(node->right)) + 1;
  }

  void _walk_preorder(Node* node, const std::function<void(Node*)>& fn) {
    if (!node) {
      return;
    }
    fn(node);
    _walk_preorder(node->left, fn);
    _walk_preorder(node->right, fn);
  }

  void _walk_inorder(Node* node, const std::function<void(Node*)>& fn) {
    if (!node) {
      return;
    }
    _walk_inorder(node->left, fn);
    fn(node);
    _walk_inorder(node->right, fn);
  }

  void _walk_postorder(Node* node, const std::function<void(Node*)>& fn) {
    if (!node) {
      return;
    }
    _walk_postorder(node->left, fn);
    _walk_postorder(node->right, fn);
    fn(node);
  }

  Node* _search(const_reference ele, Node* node) {
    if (!node || node->value == ele) {
      return node;
    } else if (node->value > ele) {
      return _search(ele, node->left);
    } else {
      return _search(ele, node->right);
    }
  }

  Node* _push(Node* node, const_reference ele) {
    if (!node) {
      return new Node{ele};
    } else if (node->value < ele) {
      node->right = _push(node->right, ele);
    } else if (node->value > ele) {
      node->left = _push(node->left, ele);
    }
    return node;
  }

  Node* _push(Node* node, rvalue_reference ele) {
    if (!node) {
      return new Node{std::move(ele)};
    } else if (node->value < ele) {
      node->right = _push(node->right, std::move(ele));
    } else if (node->value > ele) {
      node->left = _push(node->left, std::move(ele));
    }
    return node;
  }
};
} // namespace trees