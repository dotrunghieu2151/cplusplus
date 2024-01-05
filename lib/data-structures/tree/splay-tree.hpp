#pragma once

#include <algorithm>
#include <allocator.hpp>
#include <concept.hpp>
#include <functional>
#include <helpers.hpp>
#include <iostream>
#include <queue.hpp>
#include <utility>

#define SPLAY_TREE_DEBUG 0

#if SPLAY_TREE_DEBUG == 1
#define SPLAY_TREE_DEBUG_MS(mes)                                               \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define SPLAY_TREE_DEBUG_MS(mes)                                               \
  do {                                                                         \
  } while (0)
#endif

namespace trees {
template <concepts::Comparable T, typename Data,
          concepts::Allocator Allocator = Allocator<T>>
class SplayTree {
private:
  class Node;

public:
  using key_type = T;
  using value_type = Data;
  using pointer = Data*;
  using reference = Data&;
  using rvalue_reference = Data&&;
  using const_reference = const Data&;
  using self = SplayTree<T, Data, Allocator>;

  SplayTree() = default;
  ~SplayTree() {
    _walk_postorder(_root, [](Node* node) { delete node; });
  };

  SplayTree(self& other) {
    other.walk_depth_first_preorder(
        [this](const key_type& key, reference ele) { push(key, ele); });
  };
  SplayTree(self&& other) : _root{other._root} { other._root = nullptr; }

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

  template <concepts::IsSameBase<key_type> Key,
            concepts::IsSameBase<value_type> Value>
  void push(Key&& key, Value&& value) {
    if (!_root) {
      _root = new Node{std::forward<Key>(key), std::forward<Value>(value)};
      return;
    }

    _root = _splay(_root, key);

    if (_root->_key == key) {
      return;
    }

    Node* tmp{new Node{std::forward<Key>(key), std::forward<Value>(value)}};
    if (_root->_key > key) {
      tmp->_right = _root;
      tmp->_left = _root->_left;
      _root->_left = nullptr;
    } else {
      tmp->_left = _root;
      tmp->_right = _root->_right;
      _root->_right = nullptr;
    }
    _root = tmp;
    return;
  }

  void remove(const key_type& key) {
    if (!_root) {
      return;
    }

    _root = _splay(_root, key);

    if (_root->_key != key) {
      return;
    }

    Node* tmp{_root};
    std::cout << *this << "\n";

    // If key is present
    // If left child of root does not exist
    // make root->right as root
    if (!_root->_left) {
      _root = _root->_right;
    } else {
      /*
      Note: Since key == root->key,
      so after Splay(key, root->lchild),
      the tree we get will have no right child tree
      and maximum node in left subtree will get splayed
      */
      // New root
      _root = _splay(_root->_left, key);
      _root->_right = tmp->_right;
    }

    delete tmp;
    return;
  };

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
      os << "[" << node->_key << "]  ";
      --rowNodeCount;
      if (node->_left) {
        queue.push_back(node->_left);
        nextRowNodeCount += 1;
      }
      if (node->_right) {
        queue.push_back(node->_right);
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
  class Node {
    friend class SplayTree;

    Node* _left{nullptr};
    Node* _right{nullptr};
    key_type _key{};
    value_type _value{};

  public:
    Node() = default;

    template <concepts::IsSameBase<key_type> Key,
              concepts::IsSameBase<value_type> Value>
    Node(Key&& key, Value&& value)
        : _key{std::forward<Key>(key)}, _value{std::forward<Value>(value)} {}

    ~Node() {}

    Node(const Node& other) = default;
    Node& operator=(const Node& other) = default;

    Node(Node&& other) = default;
    Node& operator=(Node&& other) = default;

    int height() {
      return std::max<int>(_left ? _left->height() : -1,
                           _right ? _right->height() : -1) +
             1;
    }

    bool is_leaf() { return !_right && !_left; }

    void* operator new(std::size_t size) {
      typename Allocator::rebind<Node>::other alloc{};
      return static_cast<void*>(alloc.allocate(size));
    }

    void operator delete(void* p, std::size_t) {
      typename Allocator::rebind<Node>::other alloc{};
      alloc.deallocate(static_cast<Node*>(p));
      return;
    }

  private:
    // this method assumes that the subtree is at least 3 level deep:
    // grandparent (this node) => parent => grandchild
    // move right child to  parent, parent move to left child of right child
    // return updated subtree
    Node* left_rotate() {
      Node* rightChild{_right};
      Node* leftChildOfRightChild{rightChild->_left};

      rightChild->_left = this;
      this->_right = leftChildOfRightChild;

      return rightChild;
    }

    // this method assumes that the subtree is at least 3 level deep:
    // grandparent (this node) => parent => grandchild
    // move left child to parent, parent move to right child of left child
    // return updated subtree
    Node* right_rotate() {
      Node* leftChild{_left};
      Node* rightChildOfLeftChild{_left->_right};

      leftChild->_right = this;
      this->_left = rightChildOfLeftChild;

      return leftChild;
    }
  };

  Node* _root{nullptr};

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
    _root = _splay(node, key);
    return _root && _root->_key == key ? _root : nullptr;
  }

  Node* _splay(Node* node, const key_type& key) {
    if (!node || node->_key == key) {
      return node;
    }

    // find on the left of node
    if (node->_key > key) {
      if (!node->_left) {
        return node;
      }
      if (node->_left->_key > key) {
        node->_left->_left = _splay(node->_left->_left, key);
        node = node->right_rotate();
      } else if (node->_left->_key < key) {
        node->_left->_right = _splay(node->_left->_right, key);
        if (node->_left->_right) {
          node->_left = node->_left->left_rotate();
        }
      }
      return node->_left ? node->right_rotate() : node;
    } else {
      if (!node->_right) {
        return node;
      }
      if (node->_right->_key < key) {
        node->_right->_right = _splay(node->_right->_right, key);
        node = node->left_rotate();
      } else if (node->_right->_key > key) {
        node->_right->_left = _splay(node->_right->_left, key);
        if (node->_right->_left) {
          node->_right = node->_right->right_rotate();
        }
      }
      return node->_right ? node->left_rotate() : node;
    }
  }
};
} // namespace trees