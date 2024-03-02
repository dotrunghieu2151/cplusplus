#pragma once

#include <algorithm>
#include <allocator.hpp>
#include <concept.hpp>
#include <functional>
#include <helpers.hpp>
#include <iostream>
#include <queue.hpp>
#include <utility>

#define RBT_TREE_DEBUG 0

#if RBT_TREE_DEBUG == 1
#define RBT_TREE_DEBUG_MS(mes)                                                 \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define RBT_TREE_DEBUG_MS(mes)                                                 \
  do {                                                                         \
  } while (0)
#endif

namespace trees {
template <concepts::Comparable T, typename Data,
          concepts::Allocator Allocator = Allocator<T>>
class RBT {
private:
  class Node;

  enum Color { red, black };

public:
  using key_type = T;
  using value_type = Data;
  using pointer = Data*;
  using reference = Data&;
  using rvalue_reference = Data&&;
  using const_reference = const Data&;
  using self = RBT<T, Data, Allocator>;

  RBT() = default;
  ~RBT() {
    _walk_postorder(_root, [](Node* node) { delete node; });
  };

  RBT(self& other) {
    other.walk_depth_first_preorder(
        [this](const key_type& key, reference ele) { push(key, ele); });
  };
  RBT(self&& other) : _root{other._root} { other._root = nullptr; }

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
    _root->_color = Color::black;
  }

  void remove(const key_type& key) {
    bool hasBalanced = false;
    _root = _delete(_root, key, hasBalanced);
    if (_root) {
      _root->_color = Color::black;
    }
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

  pointer lca(const key_type& n1, const key_type& n2) {
    Node* node{_lca(_root, n1, n2)};
    return node ? &node->_value : nullptr;
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
      os << "[" << node->_key << "]" << (tree._is_red(node) ? "[R] " : "[B] ");
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
    friend class RBT;

  public:
    Color _color{Color::red};
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

    bool is_leaf() { return !_right && !_left; }

    bool is_red() { return _color == Color::red; }

    void flip_color() {
      _color = static_cast<Color>(_color ^ 1);
      if (_right) {
        _right->_color = static_cast<Color>(_right->_color ^ 1);
      }
      if (_left) {
        _left->_color = static_cast<Color>(_left->_color ^ 1);
      }
      return;
    }

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

      rightChild->_color = this->_color;
      this->_color = Color::red;

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

      leftChild->_color = this->_color;
      this->_color = Color::red;

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

  int _height(Node* node) {
    if (!node) {
      return -1;
    }
    return std::max<int>(_height(node->_left), _height(node->_right)) + 1;
  }

  template <concepts::IsSameBase<key_type> Key,
            concepts::IsSameBase<value_type> Value>
  Node* _push(Node* node, Key&& key, Value&& value) {
    if (!node) {
      return new Node{std::forward<Key>(key), std::forward<Value>(value)};
    } else if (node->_key < key) {
      node->_right = _push(node->_right, std::forward<Key>(key),
                           std::forward<Value>(value));
      return _fix_up_right(node);
    } else if (node->_key > key) {
      node->_left = _push(node->_left, std::forward<Key>(key),
                          std::forward<Value>(value));
      return _fix_up_left(node);
    } else {
      // duplicate key is not allowed, we simply returned the node
      return node;
    }
  }

  Node* _fix_up_left(Node* node) {
    if (_is_red(node->_left)) {
      // case 1: both children are red
      if (_is_red(node->_right)) {
        // check if there 2 reds children in a row (LL, LR)
        if (_is_red(node->_left->_left) || _is_red(node->_left->_right)) {
          node->flip_color();
        }
      } else if (_is_red(node->_left->_left)) {
        // 2 red left in a row => right rotate
        node = node->right_rotate();
      } else if (_is_red(node->_left->_right)) {
        // 2 red, double rotate
        node->_left = node->_left->left_rotate();
        node = node->right_rotate();
      }
    }
    return node;
  }

  Node* _fix_up_right(Node* node) {
    if (_is_red(node->_right)) {
      // case 1: both children are red
      if (_is_red(node->_left)) {
        // check if there 2 reds children in a row (RR, RL)
        if (_is_red(node->_right->_right) || _is_red(node->_right->_left)) {
          node->flip_color();
        }
      } else if (_is_red(node->_right->_right)) {
        // 2 red left in a row => right rotate
        node = node->left_rotate();
      } else if (_is_red(node->_right->_left)) {
        // 2 red, double rotate
        node->_right = node->_right->right_rotate();
        node = node->left_rotate();
      }
    }
    return node;
  }

  // delete is similar to normal BST delete
  // however we have to do more than 1 rotation (unlike insertion), because we
  // could delete any node in the tree
  // if we delete red node, we're fine. If it is a black node, the tree's black
  // depth has changes
  // we need to fix up in this case
  // Also, deletion in RBT requires fewer rotations than AVL tree
  Node* _delete(Node* node, const key_type& key, bool& hasBalanced) {
    if (!node) {
      hasBalanced = true;
      return nullptr;
    } else if (node->_key > key) {
      node->_left = _delete(node->_left, key, hasBalanced);
      return hasBalanced ? node : _fix_up_left_after_delete(node, hasBalanced);
    } else if (node->_key < key) {
      node->_right = _delete(node->_right, key, hasBalanced);
      return hasBalanced ? node : _fix_up_right_after_delete(node, hasBalanced);
    } else {
      // found ele, delete
      if (!node->_left && !node->_right) {
        // case 1: no child
        if (_is_red(node)) {
          hasBalanced = true;
        }
        delete node;
        return nullptr;
      } else if (!node->_right) {
        // case 2: 1 child
        Node* tmp{node->_left};
        if (_is_red(node)) {
          // if node is red, black depth is not changed, simply delete and
          // replace with child
          hasBalanced = true;
          delete node;
          return tmp;
        } else if (_is_red(tmp)) {
          // if child is red, delete node, and flip child color => black depth
          // not changed
          hasBalanced = true;
          delete node;
          tmp->_color = Color::black;
          return tmp;
        } else {
          // child is also black, black depth has changed, we replace node with
          // black child and let the grandparent balance
          delete node;
          return tmp;
        }
      } else if (!node->_left) {
        Node* tmp{node->_right};
        if (_is_red(node)) {
          // if node is red, black depth is not changed, simply delete and
          // replace with child
          hasBalanced = true;
          delete node;
          return tmp;
        } else if (_is_red(tmp)) {
          // if child is red, delete node, and flip child color => black depth
          // not changed
          hasBalanced = true;
          delete node;
          tmp->_color = Color::black;
          return tmp;
        } else {
          // child is also black, black depth has changed, we replace node with
          // black child and let the grandparent balance
          delete node;
          return tmp;
        }
      } else {
        // case 3: 2 children
        Node* maxLeftNode{_max(node->_left)};
        std::swap(node->_value, maxLeftNode->_value);
        std::swap(node->_key, maxLeftNode->_key);
        node->_left = _delete(node->_left, maxLeftNode->_key, hasBalanced);
        return hasBalanced ? node
                           : _fix_up_left_after_delete(node, hasBalanced);
      }
    }
  }

  Node* _fix_up_left_after_delete(Node* node, bool& hasBalanced) {
    Node* sibling{node->_right};
    if (!sibling) {
      return node;
    }
    Node* parent{node};
    // red sibling case, we rotate right by the sibling, reducing it to another
    // case: red reduction
    if (_is_red(sibling)) {
      node = node->left_rotate();
      sibling = parent->_right;
    }
    // case: sibling has both black children
    if (!_is_red(sibling->_left) && !_is_red(sibling->_right)) {
      if (_is_red(parent)) {
        hasBalanced = true;
      }
      parent->_color = Color::black;
      sibling->_color = Color::red;
    } else {
      // case: sibling has 1 red children
      Color initialParentColor{parent->_color};
      bool isRedReductionCase{node != parent};

      if (_is_red(sibling->_right)) {
        parent = parent->left_rotate();
      } else {
        parent->_right = parent->_right->right_rotate();
        parent = parent->left_rotate();
      }

      parent->_color = initialParentColor;
      parent->_left->_color = Color::black;
      parent->_right->_color = Color::black;

      if (isRedReductionCase) {
        node->_left = parent;
      } else {
        node = parent;
      }
      hasBalanced = true;
    }

    return node;
  }

  Node* _fix_up_right_after_delete(Node* node, bool& hasBalanced) {
    Node* sibling{node->_left};
    if (!sibling) {
      return node;
    }
    Node* parent{node};
    // red reduction case
    if (_is_red(sibling)) {
      node = node->right_rotate();
      sibling = parent->_left;
    }
    if (!_is_red(sibling->_left) && !_is_red(sibling->_right)) {
      if (_is_red(parent)) {
        hasBalanced = true;
      }
      parent->_color = Color::black;
      sibling->_color = Color::red;
    } else {
      Color initialParentColor{parent->_color};
      bool isRedReductionCase{node != parent};

      if (_is_red(sibling->_left)) {
        parent = parent->right_rotate();
      } else {
        parent->_left = parent->_left->left_rotate();
        parent = parent->right_rotate();
      }

      parent->_color = initialParentColor;
      parent->_left->_color = Color::black;
      parent->_right->_color = Color::black;

      if (isRedReductionCase) {
        node->_right = parent;
      } else {
        node = parent;
      }
      hasBalanced = true;
    }
    return node;
  }

  bool _is_red(Node* node) const { return node ? node->is_red() : false; }

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