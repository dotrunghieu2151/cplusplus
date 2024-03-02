#pragma once

#include <algorithm>
#include <allocator.hpp>
#include <concept.hpp>
#include <functional>
#include <helpers.hpp>
#include <iostream>
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

  template <concepts::IsSameBase<key_type> Key,
            concepts::IsSameBase<value_type> Value>
  void push(Key&& key, Value&& value) {
    _root = _push(_root, std::forward<Key>(key), std::forward<Value>(value));
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

  pointer lca(const key_type& n1, const key_type& n2) {
    Node* node{_lca(_root, n1, n2)};
    return node ? &node->_value : nullptr;
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
    friend class AVLTree;

  public:
    int _height{};
    Node* _left{nullptr};
    Node* _right{nullptr};
    key_type _key{};
    value_type _value{};

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

    int get_balance_factor() {
      int nodeLeftHeight = this->_left ? this->_left->_height : -1;
      int nodeRightHeight = this->_right ? this->_right->_height : -1;
      return nodeLeftHeight - nodeRightHeight;
    }

    bool is_subtree_balanced() {
      int balanceFactor{get_balance_factor()};
      return balanceFactor >= -1 && balanceFactor <= 1;
    }

    void update_height() {
      _height = std::max<int>(_left ? _left->height() : -1,
                              _right ? _right->height() : -1) +
                1;
      return;
    }

    int height() { return _height; }

    bool is_leaf() { return !_right && !_left; }

    void* operator new(std::size_t size) {
      typename Allocator::template rebind<Node>::other alloc{};
      return static_cast<void*>(alloc.allocate(size));
    }

    void operator delete(void* p, std::size_t) {
      typename Allocator::template rebind<Node>::other alloc{};
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

      // must update this height first, because it is now a child of rightChild
      this->update_height();
      // update rightChild height to reflect correct balance factor
      rightChild->update_height();
      // return the new subtree, ancestor of this subtree should update its left
      // (right) to point to this rightChild
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

      this->update_height();
      leftChild->update_height();

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
    if (!node || node->_key == key) {
      return node;
    } else if (node->_key > key) {
      return _search(node->_left, key);
    } else {
      return _search(node->_right, key);
    }
  }

  /* for insertion, insert normally like a BST. Then check the balance factor
  and rotate accordingly AVL tree only needs 1 rotate (double rotations like LR,
  RL still counts as 1 rotation)
  Since AVL must be balanced at all time
  when inserting, we only need to traverse the insertion path upward at most 2
  times. 1 time to parent (which height will be + 1). 1 time to grandparent
  (which height will be + 2)
  => we balance at this grandparent node
  */
  template <concepts::IsSameBase<key_type> Key,
            concepts::IsSameBase<value_type> Value>
  Node* _push(Node* node, Key&& key, Value&& value) {
    if (!node) {
      return new Node{std::forward<Key>(key), std::forward<Value>(value)};
    } else if (node->_key < key) {
      node->_right = _push(node->_right, std::forward<Key>(key),
                           std::forward<Value>(value));
    } else if (node->_key > key) {
      node->_left = _push(node->_left, std::forward<Key>(key),
                          std::forward<Value>(value));
    } else {
      // duplicate key is not allowed, we simply returned the node
      return node;
    }

    node->update_height();

    // we try to balance the tree here.
    int balanceFactor{node->get_balance_factor()};

    if (balanceFactor > 1) {
      // left subtree is heavier, need to right rotate or LR rotate
      if (key < node->_left->_key) {
        // do right rotate
        return node->right_rotate();
      } else {
        // do LR rotate
        node->_left = node->_left->left_rotate();
        return node->right_rotate();
      }
    } else if (balanceFactor < -1) {
      // right subtree is heavier, need to left rotate or RL rotate
      if (key > node->_right->_key) {
        // do left rotate
        return node->left_rotate();
      } else {
        // do RL rotate
        node->_right = node->_right->right_rotate();
        return node->left_rotate();
      }
    }
    // else node is balanced
    return node;
  }

  // delete is similar to normal BST delete
  // however we have to do more than 1 rotation (unlike insertion), because we
  // could delete any node in the tree
  // we have to recurivsely check from the deleted node to the root, and balance
  // if necessary
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

    // if we delete leaf node, return
    if (!node) {
      return nullptr;
    }

    // else we try to balance from this node to root
    node->update_height();
    int balanceFactor{node->get_balance_factor()};
    if (balanceFactor < -1) {
      // right tree is heavier, do left rotation or RL rotation
      if (node->_right->get_balance_factor() <= 0) {
        // node right subtree is heavier, rotate left
        return node->left_rotate();
      } else {
        node->_right = node->_right->right_rotate();
        return node->left_rotate();
      }
    } else if (balanceFactor > 1) {
      // left tree is heavier, do right rotation or LR rotation
      if (node->_left->get_balance_factor() >= 0) {
        // node left subtree is heavier, rotate right
        return node->right_rotate();
      } else {
        node->_left = node->_left->left_rotate();
        return node->right_rotate();
      }
    }
    return node;
  }

  Node* _lca(Node* root, const key_type& n1, const key_type& n2) {
    if (!root) {
      return nullptr;
    }

    if (root->_key > n1 && root->_key > n2) {
      return _lca(root->_left, n1, n2);
    } else if (root->_key < n1 && root->_key < n2) {
      return _lca(root->_right, n1, n2);
    } else {
      return root;
    }
  }
};
} // namespace trees