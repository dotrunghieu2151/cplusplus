#pragma once
#include <algorithm>
#include <allocator.hpp>
#include <concept.hpp>
#include <functional>
#include <helpers.hpp>
#include <iostream>
#include <queue.hpp>
#include <static-circular-buffer.hpp>
#include <utility>
#include <variant>

#define B_PLUS_DEBUG 0

#if B_PLUS_DEBUG == 1
#define B_PLUS_DEBUG_MS(mes)                                                   \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define B_PLUS_DEBUG_MS(mes)                                                   \
  do {                                                                         \
  } while (0)
#endif

namespace trees {
template <concepts::Comparable T, typename Data, std::size_t DEGREE,
          concepts::Allocator Allocator = Allocator<T>>
class BPlusTree {

private:
  inline static constexpr std::size_t minKey{DEGREE - 1};
  inline static constexpr std::size_t minChildren{DEGREE};
  inline static constexpr std::size_t maxKey{2 * DEGREE - 1};
  inline static constexpr std::size_t maxChildren{2 * DEGREE};
  // (maxKey - 1)/ 2 = minKey
  inline static constexpr std::size_t midKeyIndex{DEGREE - 1};

  class Node {
    friend BPlusTree;

  protected:
    StaticCircularBuffer<T, maxKey> _keys{};

  public:
    Node() = default;
    virtual ~Node() = default;

  public:
    bool is_full() { return _keys.size() == maxKey; }
    bool has_minimum_key() { return _keys.size() == minKey; }
    virtual bool is_leaf() = 0;

    virtual std::size_t _get_key_upper_bound_index(const T& key) = 0;

    // split self into a new node with half of the origin node, return the split
    // key and new node, the old node is modified
    virtual std::pair<T, Node*> split_self() = 0;

  protected:
    std::size_t _binary_search_key_upper_bound_index(const T& key,
                                                     std::size_t min,
                                                     std::size_t max) {
      if (min > max) {
        return min;
      }
      const std::size_t midPoint{std::midpoint(min, max)};
      if (_keys[midPoint] == key) {
        return midPoint;
      } else if (_keys[midPoint] < key) {
        return _binary_search_key_upper_bound_index(key, ++min, max);
      } else {
        return _binary_search_key_upper_bound_index(key, min, --max);
      }
    }
  };

  class NodeLeaf : public Node {
    friend BPlusTree;
    using allocator = typename Allocator::rebind<NodeLeaf>::other;

  private:
    StaticCircularBuffer<Data, maxKey> _dataArr{};
    NodeLeaf* _next{nullptr};

  public:
    NodeLeaf() = default;

    ~NodeLeaf() = default;

    NodeLeaf(const NodeLeaf&) = default;

    NodeLeaf(NodeLeaf&&) = default;

    NodeLeaf& operator=(const NodeLeaf&) = default;
    NodeLeaf& operator=(NodeLeaf&&) = default;

    bool is_leaf() override { return true; }

    std::size_t _get_key_upper_bound_index(const T& key) override {
      if (key > this->_keys.back()) {
        return this->_keys.size();
      } else if (key <= this->_keys.front()) {
        return 0;
      } else {
        return this->_binary_search_key_upper_bound_index(
            key, 1, this->_keys.size() - 1);
      }
    };

    std::pair<T, Node*> split_self() override {
      allocator alloc{};
      NodeLeaf* newRightNode{alloc.allocate(1)};
      alloc.construct(newRightNode);
      T splitKey{this->_keys[midKeyIndex]};
      // move keys
      newRightNode->_keys.insert(
          0, std::make_move_iterator(this->_keys.begin() + midKeyIndex),
          std::make_move_iterator(this->_keys.end()));

      this->_keys.erase(this->_keys.begin() + midKeyIndex, this->_keys.end());

      // move data
      newRightNode->_dataArr.insert(
          0, std::make_move_iterator(_dataArr.begin() + midKeyIndex),
          std::make_move_iterator(_dataArr.end()));

      _dataArr.erase(_dataArr.begin() + midKeyIndex, _dataArr.end());

      // add next pointer
      newRightNode->_next = _next;
      _next = newRightNode;

      return std::make_pair<T, Node*>(std::move(splitKey), newRightNode);
    }
  };

  class NodeNonLeaf : public Node {
    friend BPlusTree;

    using allocator = typename Allocator::rebind<NodeNonLeaf>::other;

  private:
    StaticCircularBuffer<Node*, maxChildren> _children{};

  public:
    NodeNonLeaf() = default;

    bool is_leaf() override { return false; }
    std::size_t _get_key_upper_bound_index(const T& key) override {
      if (key >= this->_keys.back()) {
        return this->_keys.size();
      } else if (key < this->_keys.front()) {
        return 0;
      } else {
        std::size_t keyIndex{this->_binary_search_key_upper_bound_index(
            key, 1, this->_keys.size() - 1)};
        return this->_keys[keyIndex] == key ? keyIndex + 1 : keyIndex;
      }
    };

    std::pair<T, Node*> split_self() override {
      allocator alloc{};
      NodeNonLeaf* newRightNode{alloc.allocate(1)};
      alloc.construct(newRightNode);

      // move keys
      newRightNode->_keys.insert(
          0, std::make_move_iterator(this->_keys.begin() + (midKeyIndex + 1)),
          std::make_move_iterator(this->_keys.end()));

      this->_keys.erase(this->_keys.begin() + (midKeyIndex + 1),
                        this->_keys.end());

      T splitKey{this->_keys.pop_back()};

      // move children
      newRightNode->_children.insert(
          0, std::make_move_iterator(_children.begin() + (midKeyIndex + 1)),
          std::make_move_iterator(_children.end()));

      _children.erase(_children.begin() + (midKeyIndex + 1), _children.end());

      return std::make_pair<T, Node*>(std::move(splitKey), newRightNode);
    }

    void splitChild(std::size_t keyIndex, Node* childNode) {
      auto [key, newNode] = childNode->split_self();
      this->_keys.insert(keyIndex, std::move(key));
      _children.insert(keyIndex + 1, newNode);
    }

    // assuming previous node exists and have enough keys
    void borrow_from_previous(std::size_t index) {
      // we move the parent key + data at (index - 1) to child at index
      Node* leftChild{_children[index - 1]};
      Node* child{_children[index]};

      child->_keys.push_front(this->_keys[index - 1]);

      if (child->is_leaf()) {
        static_cast<NodeLeaf*>(child)->_dataArr.push_front(
            static_cast<NodeLeaf*>(leftChild)->_dataArr.pop_back());
        if (child->_keys[0] == child->_keys[1]) {
          // duplicate key in internal node and leaf node, we replace the
          // duplicate key with the inorder predecessor key
          // fyi, this copies but not move because _keys.back() returns
          // reference
          child->_keys[0] = leftChild->_keys.back();
        }
      }

      this->_keys[index - 1] = leftChild->_keys.pop_back();

      if (!child->is_leaf()) {
        // move children too
        static_cast<NodeNonLeaf*>(child)->_children.push_front(
            static_cast<NodeNonLeaf*>(leftChild)->_children.pop_back());
      }
      return;
    }

    // assuming next node exists and have enough keys
    void borrow_from_next(std::size_t index) {
      // we move the parent key + data at index to child at index
      Node* child{_children[index]};
      Node* rightChild{_children[index + 1]};

      child->_keys.push_back(this->_keys[index]);

      this->_keys[index] = rightChild->_keys.pop_front();

      if (child->is_leaf()) {
        static_cast<NodeLeaf*>(child)->_dataArr.push_back(
            static_cast<NodeLeaf*>(rightChild)->_dataArr.pop_front());
        if (child->_keys.back() == this->_keys[index]) {
          this->_keys[index] = rightChild->_keys[0];
        }
      }

      if (!child->is_leaf()) {
        // move children too
        static_cast<NodeNonLeaf*>(child)->_children.push_back(
            static_cast<NodeNonLeaf*>(rightChild)->_children.pop_front());
      }
    }

    // merge child at index with child at index + 1
    void merge(std::size_t index) {
      Node* child{_children[index]};
      Node* rightChild{_children[index + 1]};

      if (!child->is_leaf()) {
        child->_keys.push_back(this->_keys[index]);
      }
      // merge with right child
      child->_keys.insert(child->_keys.size(),
                          std::make_move_iterator(rightChild->_keys.begin()),
                          std::make_move_iterator(rightChild->_keys.end()));

      if (!child->is_leaf()) {
        static_cast<NodeNonLeaf*>(child)->_children.insert(
            static_cast<NodeNonLeaf*>(child)->_children.size(),
            std::make_move_iterator(
                static_cast<NodeNonLeaf*>(rightChild)->_children.begin()),
            std::make_move_iterator(
                static_cast<NodeNonLeaf*>(rightChild)->_children.end()));
      } else {
        static_cast<NodeLeaf*>(child)->_dataArr.insert(
            static_cast<NodeLeaf*>(child)->_dataArr.size(),
            std::make_move_iterator(
                static_cast<NodeLeaf*>(rightChild)->_dataArr.begin()),
            std::make_move_iterator(
                static_cast<NodeLeaf*>(rightChild)->_dataArr.end()));
      }
      this->_children.erase(index + 1);
      this->_keys.erase(index);
      return;
    }

    void fill(std::size_t keyIndex) {
      if (keyIndex > 0 && !_children[keyIndex - 1]->has_minimum_key()) {
        borrow_from_previous(keyIndex);
      } else if (keyIndex + 1 < this->_children.size() &&
                 !this->_children[keyIndex + 1]->has_minimum_key()) {
        borrow_from_next(keyIndex);
      } else {
        // merge child at index with either left or right neighboring child
        // if right-most child then merge with left, else merge with right
        merge(keyIndex + 1 < this->_children.size() ? keyIndex : keyIndex - 1);
      }
      return;
    }
  };

private:
  Node* _root{nullptr};

public:
  using value_type = Data;
  using pointer = Data*;
  using reference = Data&;
  using rvalue_reference = Data&&;
  using const_reference = const Data&;
  using self = BPlusTree<T, Data, DEGREE, Allocator>;

  BPlusTree() = default;
  ~BPlusTree() {
    if (_root) {
      _walk_node_depth_first_postorder(_root, [this](Node* node) {
        if (node->is_leaf()) {
          free_leaf_node(static_cast<NodeLeaf*>(node));
        } else {
          free_non_leaf_node(static_cast<NodeNonLeaf*>(node));
        }
      });
    }
  };

  BPlusTree(self& other) {
    other.walk_depth_first_inorder(
        [this](const T& key, reference data) { insert(key, data); });
  };
  BPlusTree(self&& other) : _root{other._root} { other._root = nullptr; };

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

  void swap(self& other) noexcept {
    using std::swap;
    swap(_root, other._root);
  }

  friend void swap(self& o1, self& o2) noexcept { o1.swap(o2); }

  pointer search(const T& key) { return _root ? _search(_root, key) : nullptr; }

  Vector<std::pair<const T&, reference>> search(const T& keyStart,
                                                const T& keyEnd) {
    if (!_root) {
      return {};
    }
    Node* node{_root};
    std::size_t keyIndex{};
    // drill down to leaf node
    while (!node->is_leaf()) {
      keyIndex = node->_get_key_upper_bound_index(keyStart);
      node = static_cast<NodeNonLeaf*>(node)->_children[keyIndex];
    }
    keyIndex = node->_get_key_upper_bound_index(keyStart);
    Vector<std::pair<const T&, reference>> result{};
    while (node) {
      for (; keyIndex < node->_keys.size(); ++keyIndex) {
        if (node->_keys[keyIndex] > keyEnd) {
          return result;
        }
        result.push_back({node->_keys[keyIndex],
                          static_cast<NodeLeaf*>(node)->_dataArr[keyIndex]});
      }
      node = static_cast<NodeLeaf*>(node)->_next;
      keyIndex = 0;
    }

    return result;
  }

  template <typename U> void insert(const T& key, U&& data) {
    if (!_root) {
      _root = alloc_leaf_node();
      _root->_keys.push_back(key);
      static_cast<NodeLeaf*>(_root)->_dataArr.push_back(std::forward<U>(data));
    } else {
      if (_root->is_full()) {
        // we split root, tree will increase height
        NodeNonLeaf* newRoot{alloc_non_leaf_node()};
        newRoot->_children.push_back(_root);
        newRoot->splitChild(0, _root);
        _root = newRoot;
      }
      return _insert_non_root(_root, key, std::forward<U>(data));
    }
  }

  void remove(const T& key) {
    if (!_root) {
      return;
    }

    _delete(_root, key);

    if (_root->_keys.empty()) {
      // the tree shrinks
      Node* tmpRoot{_root};
      if (tmpRoot->is_leaf()) {
        _root = nullptr;
      } else {
        _root = static_cast<NodeNonLeaf*>(tmpRoot)->_children.pop_back();
      }
      delete tmpRoot;
    }
    return;
  }

  void walk_breadth_first(std::function<void(const T&, reference)> fn) {
    if (!_root) {
      return;
    }
    Queue<Node*> queue{};
    queue.push_back(_root);
    while (!queue.empty()) {
      Node* node{queue.pop_front()};
      for (std::size_t i{}; i < node->_keys.size(); ++i) {
        fn(node->_keys[i], node->_dataArr[i]);
      }
      if (!node->is_leaf()) {
        for (std::size_t i{}; i < node->_children.size(); ++i) {
          queue.push_back(node->_children[i]);
        }
      }
    }
    return;
  }

  // inorder traverse
  void walk_depth_first_inorder(std::function<void(const T&, reference)> fn) {
    if (!_root) {
      return;
    }
    _walk_depth_first_inorder(_root, fn);
  }

  int height() { return _height(_root); }

  std::pair<const T&, reference> min() { return _min(_root); }
  std::pair<const T&, reference> max() { return _max(_root); }

  friend std::ostream& operator<<(std::ostream& os, const self& bPlusTree) {
    if (!bPlusTree._root) {
      return os;
    }
    Queue<Node*> queue{};
    int rowNodeCount{1};
    int nextRowNodeCount{};
    queue.push_back(bPlusTree._root);
    while (!queue.empty()) {
      Node* node{queue.pop_front()};
      std::size_t i{};
      os << "[" << node->_keys[i];
      for (++i; i < node->_keys.size(); ++i) {
        os << ", " << node->_keys[i];
      }
      os << "]  ";
      --rowNodeCount;
      if (!node->is_leaf()) {
        for (std::size_t index{};
             index < static_cast<NodeNonLeaf*>(node)->_children.size();
             ++index) {
          queue.push_back(static_cast<NodeNonLeaf*>(node)->_children[index]);
        }
        nextRowNodeCount +=
            static_cast<int>(static_cast<NodeNonLeaf*>(node)->_children.size());
      }
      if (rowNodeCount == 0) {
        os << "\n";
        rowNodeCount = nextRowNodeCount;
        nextRowNodeCount = 0;
      }
    }
    return os;
  };

private:
  NodeLeaf* alloc_leaf_node() {
    typename NodeLeaf::allocator allocator{};
    NodeLeaf* newNode{allocator.allocate(1)};
    allocator.construct(newNode);
    return newNode;
  }

  void free_leaf_node(NodeLeaf* node) {
    typename NodeLeaf::allocator allocator{};
    allocator.destruct(node);
    allocator.deallocate(node);
  }

  NodeNonLeaf* alloc_non_leaf_node() {
    typename NodeNonLeaf::allocator allocator{};
    NodeNonLeaf* newNode{allocator.allocate(1)};
    allocator.construct(newNode);
    return newNode;
  }

  void free_non_leaf_node(NodeNonLeaf* node) {
    typename NodeNonLeaf::allocator allocator{};
    allocator.destruct(node);
    allocator.deallocate(node);
  }

  // assuming node is not leaf
  // std::pair<const T&, reference> _get_predecessor(Node* node,
  //                                                 std::size_t keyIndex) const
  //                                                 {
  //   return _max(node->_children[keyIndex]);
  // }

  // assuming node is not leaf
  // std::pair<const T&, reference> _get_successor(Node* node,
  //                                               std::size_t keyIndex) const {
  //   return _min(node->_children[keyIndex + 1]);
  // }

  void _delete(Node* node, const T& key,
               NodeNonLeaf* nodeNoneLeafWithKey = nullptr,
               std::size_t keyIndexInNodeNonLeaf = 0) {
    std::size_t keyIndex{node->_get_key_upper_bound_index(key)};
    if (node->is_leaf() && node->_keys[keyIndex] == key) {
      // case 1
      node->_keys.erase(keyIndex);
      static_cast<NodeLeaf*>(node)->_dataArr.erase(keyIndex);
      if (nodeNoneLeafWithKey) {
        // replace internal node with inorder successor
        nodeNoneLeafWithKey->_keys[keyIndexInNodeNonLeaf] =
            node->_keys[keyIndex];
      }
    } else if (!node->is_leaf()) {
      if (keyIndex > 0 && node->_keys[keyIndex - 1] == key) {
        nodeNoneLeafWithKey = static_cast<NodeNonLeaf*>(node);
        keyIndexInNodeNonLeaf = keyIndex - 1;
      }

      if (static_cast<NodeNonLeaf*>(node)
              ->_children[keyIndex]
              ->has_minimum_key()) {
        // case 3: pro-active fill child with sufficient keys
        static_cast<NodeNonLeaf*>(node)->fill(keyIndex);
      }
      bool isKeyAtLastChildAndChildIsMerged(
          keyIndex > static_cast<NodeNonLeaf*>(node)->_children.size() - 1);
      std::size_t childIndex(isKeyAtLastChildAndChildIsMerged ? keyIndex - 1
                                                              : keyIndex);
      _delete(static_cast<NodeNonLeaf*>(node)->_children[childIndex], key,
              nodeNoneLeafWithKey, keyIndexInNodeNonLeaf);
    }
  }

  template <typename U>
  void _insert_non_root(Node* node, const T& key, U&& data) {
    std::size_t keyIndex{node->_get_key_upper_bound_index(key)};
    if (node->is_leaf()) {
      // insert because we ensure there is always extra space by pro-actively
      // spliting nodes
      node->_keys.insert(keyIndex, key);
      static_cast<NodeLeaf*>(node)->_dataArr.insert(keyIndex,
                                                    std::forward<U>(data));
    } else {
      // non-leaf node, need to check
      // if node is full =>  split
      // else find next child to traverse
      Node* childNode{static_cast<NodeNonLeaf*>(node)->_children[keyIndex]};
      if (childNode->is_full()) {
        static_cast<NodeNonLeaf*>(node)->splitChild(keyIndex, childNode);
        // find the node which the key belongs to (because we split it in 2)
        if (key > node->_keys[keyIndex]) {
          ++keyIndex;
        }
      }
      return _insert_non_root(
          static_cast<NodeNonLeaf*>(node)->_children[keyIndex], key,
          std::forward<U>(data));
    }
  }

  pointer _search(Node* node, const T& key) {
    std::size_t keyIndex{node->_get_key_upper_bound_index(key)};
    if (node->is_leaf()) {
      return keyIndex < node->_keys.size() && node->_keys[keyIndex] == key
                 ? &static_cast<NodeLeaf*>(node)->_dataArr[keyIndex]
                 : nullptr;
    }
    return _search(static_cast<NodeNonLeaf*>(node)->_children[keyIndex], key);
  }

  void _walk_depth_first_inorder(
      Node* node, const std::function<void(const T&, reference)>& fn) {
    NodeLeaf* leafNode{_min_node(node)};
    while (leafNode) {
      for (std::size_t i{}; i < leafNode->_keys.size(); ++i) {
        fn(leafNode->_keys[i], leafNode->_dataArr[i]);
      }
      leafNode = leafNode->_next;
    }
  }

  int _height(Node* node) {
    return (!node || node->is_leaf())
               ? 0
               : _height(static_cast<NodeNonLeaf*>(node)->_children[0]) + 1;
  }

  NodeLeaf* _min_node(Node* node) const {
    if (!node) {
      return nullptr;
    }
    if (node->is_leaf()) {
      return static_cast<NodeLeaf*>(node);
    } else {
      return _min_node(static_cast<NodeNonLeaf*>(node)->_children[0]);
    }
  }

  std::pair<const T&, reference> _min(Node* node) const {
    if (node->is_leaf()) {
      return {node->_keys[0], static_cast<NodeLeaf*>(node)->_dataArr[0]};
    } else {
      return _min(static_cast<NodeNonLeaf*>(node)->_children[0]);
    }
  }

  std::pair<const T&, reference> _max(Node* node) const {
    if (node->is_leaf()) {
      return {node->_keys.back(),
              static_cast<NodeLeaf*>(node)->_dataArr.back()};
    } else {
      return _max(static_cast<NodeNonLeaf*>(node)->_children.back());
    }
  }

  void _walk_node_depth_first_postorder(Node* node,
                                        const std::function<void(Node*)>& fn) {
    if (!node->is_leaf()) {
      for (std::size_t i{};
           i < static_cast<NodeNonLeaf*>(node)->_children.size(); ++i) {
        _walk_node_depth_first_postorder(
            static_cast<NodeNonLeaf*>(node)->_children[i], fn);
      }
      fn(node);
    } else {
      fn(node);
    }
  }
};
} // namespace trees