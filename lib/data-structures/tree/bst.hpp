#pragma once
#include <algorithm>
#include <concept.hpp>
#include <functional>
#include <helpers.hpp>
#include <iostream>
#include <queue.hpp>
#include <utility>

#define BST_DEBUG 0

#if BST_DEBUG == 1
#define BST_DEBUG_MS(mes)                                                      \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define BST_DEBUG_MS(mes)                                                      \
  do {                                                                         \
  } while (0)
#endif

namespace trees {
template <concepts::Comparable T> class BST {
private:
  struct Node;

public:
  using value_type = T;
  using pointer = T*;
  using reference = T&;
  using rvalue_reference = T&&;
  using const_reference = const T&;
  using self = BST<T>;

  BST() = default;
  ~BST() {
    _walk_postorder(_root, [](Node* node) { delete node; });
  };

  BST(self& other) {
    other.walk_depth_first_preorder([this](reference ele) { push(ele); });
  };
  BST(self&& other) : _root{other._root} { other._root = nullptr; };

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

  void push(const_reference ele) { _root = _push(_root, ele); };
  void push(rvalue_reference ele) { _root = _push(_root, std::move(ele)); };

  void remove(const_reference ele) { _root = _delete(_root, ele); };

  pointer search(const_reference ele) {
    Node* result = _search(ele, _root);
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

  pointer get_next_inorder(const_reference ele) {
    Node* node{_inorder_successor(_root, ele)};
    return node ? &(node->value) : nullptr;
  }

  pointer get_previous_inorder(const_reference ele) {
    Node* node{_inorder_predecessor(_root, ele)};
    return node ? &(node->value) : nullptr;
  }

  int height() { return _height(_root); }

  void walk_breadth_first(std::function<void(reference)> fn) {
    Queue<Node*> queue(5);
    if (!_root) {
      return;
    }
    queue.push_back(_root);
    while (!queue.empty()) {
      Node* current{queue.pop_front()};
      fn(current->value);
      if (current->left) {
        queue.push_back(current->left);
      }
      if (current->right) {
        queue.push_back(current->right);
      }
    }
  }

  void walk_depth_first_preorder(std::function<void(reference)> fn) {
    _walk_preorder(_root, [&fn](Node* node) { fn(node->value); });
  }

  void walk_depth_first_inorder(std::function<void(reference)> fn) {
    _walk_inorder(_root, [&fn](Node* node) { fn(node->value); });
  }

  void walk_depth_first_postorder(std::function<void(reference)> fn) {
    _walk_postorder(_root, [&fn](Node* node) { fn(node->value); });
  }

  bool is_binary_search_tree() { return _is_bst(_root, nullptr, nullptr); }

  pointer lca(const_reference n1, const_reference n2) {
    Node* node{_lca(_root, n1, n2)};
    return node ? &node->value : nullptr;
  }

  void swap(self& other) noexcept {
    using std::swap;
    swap(_root, other._root);
  }
  friend void swap(self& e1, self& e2) { e1.swap(e2); };

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
      os << "[" << node->value << "]  ";
      --rowNodeCount;
      if (node->left) {
        queue.push_back(node->left);
        nextRowNodeCount += 1;
      }
      if (node->right) {
        queue.push_back(node->right);
        nextRowNodeCount += 1;
      }
      if (rowNodeCount == 0) {
        os << "\n";
        rowNodeCount = nextRowNodeCount;
        nextRowNodeCount = 0;
      }
    }
    return os;
  }

private:
  struct Node {
    value_type value{};
    Node* left{nullptr};
    Node* right{nullptr};
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
  Node* _inorder_successor(Node* node, const_reference ele) {
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

  Node* _lca(Node* root, const_reference n1, const_reference n2) {
    if (!root) {
      return nullptr;
    }

    if (root->value > n1 && root->value > n2) {
      return _lca(root->left, n1, n2);
    } else if (root->value < n1 && root->value < n2) {
      return _lca(root->right, n1, n2);
    } else {
      return root;
    }
  }
};
} // namespace trees