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

#define B_TREE_DEBUG 0

#if B_TREE_DEBUG == 1
#define B_TREE_DEBUG_MS(mes)                                                   \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define B_TREE_DEBUG_MS(mes)                                                   \
  do {                                                                         \
  } while (0)
#endif

namespace trees {
template <concepts::Comparable T, typename Data, std::size_t DEGREE,
          template <typename> class Allocator = Allocator>
class BTree {

private:
  inline static constexpr std::size_t minKey{DEGREE - 1};
  inline static constexpr std::size_t minChildren{DEGREE};
  inline static constexpr std::size_t maxKey{2 * DEGREE - 1};
  inline static constexpr std::size_t maxChildren{2 * DEGREE};
  // (maxKey - 1)/ 2 = minKey
  inline static constexpr std::size_t midKeyIndex{DEGREE - 1};

public:
  class Node {
    friend BTree;

    StaticCircularBuffer<Node*, maxChildren> _children{};
    StaticCircularBuffer<T, maxKey> _keys{};
    StaticCircularBuffer<Data, maxKey> _dataArr{};

  private:
    Node() = default;

  public:
    bool is_full() { return _keys.size() == maxKey; }
    bool has_minimum_key() { return _keys.size() == minKey; }
    bool has_minimum_children() { return _children.size() == minChildren; }
    bool is_leaf() { return _children.empty(); }

    // assuming previous node exists and have enough keys
    void borrow_from_previous(std::size_t index) {
      // we move the parent key + data at (index - 1) to child at index
      Node* leftChild{_children[index - 1]};
      Node* child{_children[index]};

      child->_keys.push_front(_keys[index - 1]);
      child->_dataArr.push_front(std::move(_dataArr[index - 1]));

      _keys[index - 1] = leftChild->_keys.pop_back();
      _dataArr[index - 1] = leftChild->_dataArr.pop_back();

      if (!child->is_leaf()) {
        // move children too
        child->_children.push_front(leftChild->_children.pop_back());
      }
      return;
    }

    // assuming next node exists and have enough keys
    void borrow_from_next(std::size_t index) {
      // we move the parent key + data at index to child at index
      Node* child{_children[index]};
      Node* rightChild{_children[index + 1]};

      child->_keys.push_back(_keys[index]);
      child->_dataArr.push_back(std::move(_dataArr[index]));

      _keys[index] = rightChild->_keys.pop_front();
      _dataArr[index] = rightChild->_dataArr.pop_front();
      if (!child->is_leaf()) {
        // move children too
        child->_children.push_back(rightChild->_children.pop_front());
      }
    }

    // fill child at index with a sufficient number of keys
    // assuming child at index is at minimum key (t-1)
    // assuming this node is not leaf
    void fill(std::size_t index) {
      if (index > 0 && !_children[index - 1]->has_minimum_key()) {
        // case 1.1 borrow from neighboring left child
        borrow_from_previous(index);
      } else if (index + 1 < _children.size() &&
                 !_children[index + 1]->has_minimum_key()) {
        // case 1.2 borrow from neighboring right child
        borrow_from_next(index);
      } else {
        // case 1.3 merge child at index with either left or right neighboring
        // child
        // if right-most child then merge with left, else merge with right
        merge(index + 1 < _children.size() ? index : index - 1);
      }
      return;
    }

    // merge child at index with child at index + 1
    void merge(std::size_t index) {
      Node* child{_children[index]};
      Node* rightChild{_children[index + 1]};

      // move key at index to left child
      child->_keys.push_back(_keys[index]);
      child->_dataArr.push_back(std::move(_dataArr[index]));

      // merge with right child
      child->_keys.insert(child->_keys.size(),
                          std::make_move_iterator(rightChild->_keys.begin()),
                          std::make_move_iterator(rightChild->_keys.end()));
      child->_dataArr.insert(
          child->_dataArr.size(),
          std::make_move_iterator(rightChild->_dataArr.begin()),
          std::make_move_iterator(rightChild->_dataArr.end()));
      if (!child->is_leaf()) {
        child->_children.insert(
            child->_children.size(),
            std::make_move_iterator(rightChild->_children.begin()),
            std::make_move_iterator(rightChild->_children.end()));
      }

      _children.erase(index + 1);
      delete rightChild;
      _keys.erase(index);
      _dataArr.erase(index);
      return;
    }

    // assuming child at keyIndex is full
    void splitChild(std::size_t keyIndex, Node* childNode) {
      Node* newRightNode{new Node{}};

      // move keys
      newRightNode->_keys.insert(
          0,
          std::make_move_iterator(childNode->_keys.begin() + (midKeyIndex + 1)),
          std::make_move_iterator(childNode->_keys.end()));

      childNode->_keys.erase(childNode->_keys.begin() + (midKeyIndex + 1),
                             childNode->_keys.end());
      T keyToMoveToParent{childNode->_keys.pop_back()};

      // move data
      newRightNode->_dataArr.insert(
          0,
          std::make_move_iterator(childNode->_dataArr.begin() +
                                  (midKeyIndex + 1)),
          std::make_move_iterator(childNode->_dataArr.end()));

      childNode->_dataArr.erase(childNode->_dataArr.begin() + (midKeyIndex + 1),
                                childNode->_dataArr.end());
      Data DataToMoveToParent{childNode->_dataArr.pop_back()};
      // move children
      if (!childNode->is_leaf()) {
        newRightNode->_children.insert(
            0,
            std::make_move_iterator(childNode->_children.begin() +
                                    (midKeyIndex + 1)),
            std::make_move_iterator(childNode->_children.end()));

        childNode->_children.erase(childNode->_children.begin() +
                                       (midKeyIndex + 1),
                                   childNode->_children.end());
      }
      // move key to parent
      _keys.insert(keyIndex, std::move(keyToMoveToParent));
      _dataArr.insert(keyIndex, std::move(DataToMoveToParent));
      _children.insert(keyIndex + 1, newRightNode);
    }
  };

private:
  Allocator<Node> _allocator{};
  Node* _root{nullptr};

public:
  using value_type = Data;
  using pointer = Data*;
  using reference = Data&;
  using rvalue_reference = Data&&;
  using const_reference = const Data&;
  using self = BTree<T, Data, DEGREE, Allocator>;

  BTree() = default;
  ~BTree() {
    if (_root) {
      _walk_node_depth_first_postorder(_root, [](Node* node) { delete node; });
    }
  };

  BTree(self& other) {
    other.walk_depth_first_inorder(
        [this](const T& key, reference data) { insert(key, data); });
  };
  BTree(self&& other) : _root{other._root} { other._root = nullptr; };

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

  Node* searchNode(const T& key) {
    return _root ? _searchNode(_root, key) : nullptr;
  }

  template <typename U> void insert(const T& key, U&& data) {
    if (!_root) {
      _root = new Node{};
      _root->_keys.push_back(key);
      _root->_dataArr.push_back(std::forward<U>(data));
    } else {
      if (_root->is_full()) {
        // we split root, tree will increase height
        Node* newRoot{new Node{}};
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
        _root = tmpRoot->_children.pop_back();
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

  // preorder traverse
  void walk_depth_first_preorder(std::function<void(const T&, reference)> fn) {
    if (!_root) {
      return;
    }
    _walk_depth_first_preorder(_root, fn);
  }

  // postorder traverse
  void walk_depth_first_postorder(std::function<void(const T&, reference)> fn) {
    if (!_root) {
      return;
    }
    _walk_depth_first_postorder(_root, fn);
  }

  int height() { return _height(_root); }

  std::pair<const T&, reference> min() { return _min(_root); }
  std::pair<const T&, reference> max() { return _max(_root); }

  friend std::ostream& operator<<(std::ostream& os, const self& btree) {
    if (!btree._root) {
      return os;
    }
    Queue<Node*> queue{};
    int rowNodeCount{1};
    int nextRowNodeCount{};
    queue.push_back(btree._root);
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
        for (std::size_t index{}; index < node->_children.size(); ++index) {
          queue.push_back(node->_children[index]);
        }
        nextRowNodeCount += static_cast<int>(node->_children.size());
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
  // assuming node is not leaf
  std::pair<const T&, reference> _get_predecessor(Node* node,
                                                  std::size_t keyIndex) const {
    return _max(node->_children[keyIndex]);
  }

  // assuming node is not leaf
  std::pair<const T&, reference> _get_successor(Node* node,
                                                std::size_t keyIndex) const {
    return _min(node->_children[keyIndex + 1]);
  }

  void _delete(Node* node, const T& key) {
    std::size_t keyIndex{_find_key_upper_bound_index(node, key)};
    if (node->_keys[keyIndex] == key) {
      if (node->is_leaf()) {
        // case 1
        node->_keys.erase(keyIndex);
        node->_dataArr.erase(keyIndex);
      } else {
        // case 2
        // check if left child has spare keys, if so delete key and swap it with
        // the the predecessor key
        if (!node->_children[keyIndex]->has_minimum_key()) {
          auto [predecessorKey, predecessorData] =
              _get_predecessor(node, keyIndex);
          node->_keys[keyIndex] = predecessorKey;
          node->_dataArr[keyIndex] = std::move(predecessorData);
          _delete(node->_children[keyIndex], predecessorKey);
        } else if (!node->_children[keyIndex + 1]->has_minimum_key()) {
          // check if right child has spare keys, if so delete key and
          // swap it with the the successor key
          auto [successorKey, successorData] = _get_successor(node, keyIndex);
          node->_keys[keyIndex] = successorKey;
          node->_dataArr[keyIndex] = std::move(successorData);
          _delete(node->_children[keyIndex + 1], successorKey);
        } else {
          // if both children have minimum keys => merge these 2 children and
          // k total key is 2t - 1, then delete k from the new node => 2t - 2
          node->merge(keyIndex);
          _delete(node->_children[keyIndex], key);
        }
      }
    } else {
      if (node->is_leaf()) {
        // no key is found
        return;
      } else {
        if (node->_children[keyIndex]->has_minimum_key()) {
          // case 3: pro-active fill child with sufficient keys
          node->fill(keyIndex);
        }
        bool isKeyAtLastChildAndChildIsMerged(keyIndex >
                                              node->_children.size() - 1);
        std::size_t childIndex(isKeyAtLastChildAndChildIsMerged ? keyIndex - 1
                                                                : keyIndex);
        _delete(node->_children[childIndex], key);
      }
    }
  }

  template <typename U>
  void _insert_non_root(Node* node, const T& key, U&& data) {
    std::size_t keyIndex{_find_key_upper_bound_index(node, key)};
    if (node->is_leaf()) {
      // insert because we ensure there is always extra space by pro-actively
      // spliting nodes
      node->_keys.insert(keyIndex, key);
      node->_dataArr.insert(keyIndex, std::forward<U>(data));
    } else {
      // non-leaf node, need to check
      // if node is full =>  split
      // else find next child to traverse
      Node* childNode{node->_children[keyIndex]};
      if (childNode->is_full()) {
        node->splitChild(keyIndex, childNode);
        // find the node which the key belongs to (because we split it in 2)
        if (key > node->_keys[keyIndex]) {
          ++keyIndex;
        }
      }
      return _insert_non_root(node->_children[keyIndex], key,
                              std::forward<U>(data));
    }
  }

  std::size_t _find_key_upper_bound_index(Node* node, const T& key) {
    if (key > node->_keys.back()) {
      return node->_keys.size();
    } else if (key <= node->_keys.front()) {
      return 0;
    } else {
      return _binary_search_key_upper_bound_index(node, key, 1,
                                                  node->_keys.size() - 1);
    }
  }
  std::size_t _binary_search_key_upper_bound_index(Node* node, const T& key,
                                                   std::size_t min,
                                                   std::size_t max) {
    if (min > max) {
      return min;
    }
    const std::size_t midPoint{std::midpoint(min, max)};
    if (node->_keys[midPoint] == key) {
      return midPoint;
    } else if (node->_keys[midPoint] < key) {
      return _binary_search_key_upper_bound_index(node, key, ++min, max);
    } else {
      return _binary_search_key_upper_bound_index(node, key, min, --max);
    }
  }

  pointer _search(Node* node, const T& key) {
    std::size_t keyIndex{_find_key_upper_bound_index(node, key)};

    if (keyIndex < node->_keys.size() && node->_keys[keyIndex] == key) {
      // we have found our key / data
      return &node->_dataArr[keyIndex];
    }
    // i is the index of the upper bound key of k
    if (node->is_leaf()) {
      // no child to keep searching, we end here
      return nullptr;
    }
    // there is a right-most child, let's drill down.
    return _search(node->_children[keyIndex], key);
  }
  Node* _searchNode(Node* node, const T& key) {
    std::size_t keyIndex{_find_key_upper_bound_index(node, key)};

    if (keyIndex < node->_keys.size() && node->_keys[keyIndex] == key) {
      return node;
    }
    if (node->is_leaf()) {
      return nullptr;
    }
    return searchNode(node->_children[keyIndex], key);
  }

  void _walk_depth_first_inorder(
      Node* node, const std::function<void(const T&, reference)>& fn) {
    if (!node->is_leaf()) {
      std::size_t i{};
      while (i < node->_keys.size()) {
        _walk_depth_first_inorder(node->_children[i], fn);
        fn(node->_keys[i], node->_dataArr[i]);
        ++i;
      }
      _walk_depth_first_inorder(node->_children[i], fn);
    } else {
      for (std::size_t i{}; i < node->_keys.size(); ++i) {
        fn(node->_keys[i], node->_dataArr[i]);
      }
    }
  }

  void _walk_depth_first_preorder(
      Node* node, const std::function<void(const T&, reference)>& fn) {
    if (!node->is_leaf()) {
      std::size_t i{};
      while (i < node->_keys.size()) {
        fn(node->_keys[i], node->_dataArr[i]);
        _walk_depth_first_preorder(node->_children[i], fn);
        ++i;
      }
      _walk_depth_first_preorder(node->_children[i], fn);
    } else {
      for (std::size_t i{}; i < node->_keys.size(); ++i) {
        fn(node->_keys[i], node->_dataArr[i]);
      }
    }
  }

  void _walk_depth_first_postorder(
      Node* node, const std::function<void(const T&, reference)>& fn) {
    if (!node->is_leaf()) {
      for (std::size_t i{}; i < node->_children.size(); ++i) {
        _walk_depth_first_postorder(node->_children[i], fn);
      }
      for (std::size_t i{}; i < node->_keys.size(); ++i) {
        fn(node->_keys[i], node->_dataArr[i]);
      }
    } else {
      for (std::size_t i{}; i < node->_keys.size(); ++i) {
        fn(node->_keys[i], node->_dataArr[i]);
      }
    }
  }

  int _height(Node* node) {
    return (!node || node->is_leaf()) ? 0 : _height(node->_children[0]) + 1;
  }

  std::pair<const T&, reference> _min(Node* node) const {
    if (node->is_leaf()) {
      return {node->_keys[0], node->_dataArr[0]};
    } else {
      return _min(node->_children[0]);
    }
  }

  std::pair<const T&, reference> _max(Node* node) const {
    if (node->is_leaf()) {
      return {node->_keys.back(), node->_dataArr.back()};
    } else {
      return _max(node->_children.back());
    }
  }

  void _walk_node_depth_first_postorder(Node* node,
                                        const std::function<void(Node*)>& fn) {
    if (!node->is_leaf()) {
      for (std::size_t i{}; i < node->_children.size(); ++i) {
        _walk_node_depth_first_postorder(node->_children[i], fn);
      }
      fn(node);
    } else {
      fn(node);
    }
  }
};
} // namespace trees