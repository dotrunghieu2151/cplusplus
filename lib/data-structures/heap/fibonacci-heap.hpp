#pragma once

#include <allocator.hpp>
#include <cmath>
#include <concept.hpp>
#include <cstddef>
#include <helpers.hpp>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <queue.hpp>
#include <string>
#include <utility>
#include <vector.hpp>
#include <vector>

#define FIBONACCI_DEBUG 1

#if FIBONACCI_DEBUG == 1
#define FIBONACCI_DEBUG_MS(mes)                                                \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define FIBONACCI_DEBUG_MS(mes)                                                \
  do {                                                                         \
  } while (0)
#endif

template <concepts::Comparable Key, typename Value,
          typename Comparator = std::less_equal<Key>,
          concepts::Allocator Allocator = Allocator<Key>>
class FibonacciHeap {
private:
  static constexpr Comparator compFn{};

public:
  using value_type = Value;
  using pointer = value_type*;
  using reference = value_type&;
  using rvalue_reference = value_type&&;
  using const_reference = const value_type&;
  using self = FibonacciHeap<Key, Value, Comparator, Allocator>;

private:
  class Node {
    friend class FibonacciHeap;

    bool _isMarked{false};
    int _degree{};
    Node* _parent{nullptr}; // Parent pointer
    Node* _child{nullptr};  // Child pointer
    Node* _left{nullptr};   // Pointer to the node on the left
    Node* _right{nullptr};  // Pointer to the node on the right

    Key _key{};
    Value _value{};

  public:
    Node() = default;

    template <concepts::IsSameBase<Key> K, concepts::IsSameBase<Value> V>
    Node(K&& key, V&& value)
        : _left{this}, _right{this}, _key{std::forward<Key>(key)},
          _value{std::forward<Value>(value)} {}

    ~Node() {}

    Node(const Node& other) = default;
    Node& operator=(const Node& other) = default;

    Node(Node&& other) = default;
    Node& operator=(Node&& other) = default;

    void* operator new(std::size_t size) {
      typename Allocator::rebind<Node>::other alloc{};
      return static_cast<void*>(alloc.allocate(size));
    }

    void operator delete(void* p, std::size_t) {
      typename Allocator::rebind<Node>::other alloc{};
      alloc.deallocate(static_cast<Node*>(p));
      return;
    }

    bool is_single_child() const { return _right == this; }
    bool has_children() const { return _child; }
    bool has_parent() const { return _parent; }
  };

  int _numNodes{};
  Node* _minNode{nullptr};

public:
  FibonacciHeap() = default;

  ~FibonacciHeap() {
    FIBONACCI_DEBUG_MS("HEAP Dtor");
    _delete_all(_minNode);
  };

  FibonacciHeap(const self& other) = default;

  FibonacciHeap(self&& other) = default;

  self& operator=(const self& other) {
    FIBONACCI_DEBUG_MS("FibonacciHeap Copy Operator");
    self copy{other};
    copy.swap(*this);
    return *this;
  };

  self& operator=(self&& other) noexcept {
    FIBONACCI_DEBUG_MS("FibonacciHeap Move Operator");
    self move{std::move(other)};
    move.swap(*this);
    return *this;
  };

  void swap(self& other) noexcept {
    using std::swap;
    swap(_numNodes, other._numNodes);
    swap(_minNode, other._minNode);
  }

  void friend swap(self& e1, self& e2) noexcept { e1.swap(e2); };

  friend std::ostream& operator<<(std::ostream& os, const self& heap) {
    if (!heap._minNode) {
      return os;
    }
    heap._print_tree(os, heap._minNode);
    return os;
  }

  int size() const noexcept { return _numNodes; }
  bool empty() const noexcept { return _numNodes == 0; }

  template <concepts::IsSameBase<Key> K, concepts::IsSameBase<Value> V>
  void insert(K&& key, V&& value) {
    _insert(std::forward<K>(key), std::forward<V>(value));
    return;
  }

  void merge(FibonacciHeap&& other) {
    _minNode = _merge(_minNode, other._minNode);
    _numNodes += other._numNodes;
    other._minNode = nullptr;
    other._numNodes = 0;
  }

  pointer top() const { return _minNode ? &_minNode->_value : nullptr; }

  pointer find(const Key& key) const {
    Node* node{_find(_minNode, key)};
    return node ? &node->_value : nullptr;
  }

  std::pair<Key, Value> extract_top() {
    Node* oldMin{_minNode};
    _minNode = _extract_top(_minNode);
    std::pair<Key, Value> result{std::move(oldMin->_key),
                                 std::move(oldMin->_value)};
    delete oldMin;
    return result;
  }

  template <concepts::IsSameBase<Key> K>
  void update_key(const Key& oldkey, K&& newKey) {
    Node* node{_find(_minNode, oldkey)};
    if (!node) {
      return;
    }
    _minNode = _update_key(_minNode, node, std::forward<K>(newKey));
    return;
  }

  void delete_key(const Key& key) {
    Node* node{_find(_minNode, key)};
    if (!node) {
      return;
    }
    Node* oldMin{_minNode};
    _update_key(_minNode, node, _minNode->_key);
    _minNode = node;
    extract_top();
    _minNode = oldMin;
  }

private:
  Node* _find(Node* root, const Key& key) const {
    if (!root) {
      return nullptr;
    }
    Node* node{root};
    do {
      if (node->_key == key) {
        return node;
      }
      Node* find{_find(node->_child, key)};
      if (find) {
        return find;
      }
      node = node->_right;
    } while (node != root);
    return nullptr;
  }

  // note: before calling this function, client should store a pointer to
  // Node* node (which is the current min).
  // This function breaks the link between
  // current min node and return new min node while ensuring that no tree has
  // the same degree
  Node* _extract_top(Node* node) {
    // break the link between this node and its subtree
    _unmark_and_unparent_all(node->_child);
    if (node->is_single_child()) {
      // node has no sibling, move child up
      node = node->_child;
    } else {
      // node has sibling, move child up next to sibling
      node->_right->_left = node->_left;
      node->_left->_right = node->_right;
      node = _merge(node->_right, node->_child);
    }

    // after extracting if tree is empty, exit
    if (!node) {
      return nullptr;
    }

    node = _consolidate(node);
    --_numNodes;

    // find the new min
    Node* newMin{node};
    Node* start{node};
    do {
      if (compFn(node->_key, newMin->_key)) {
        newMin = node;
      }
      node = node->_right;
    } while (node != start);
    return newMin;
  }

  Node* _consolidate(Node* node) {
    // get maximum degree in a heap with n nodes
    // maximum degree of heap(n) is log2(n)
    // to find log of a to any base b we divide log: log(a) / log(b) = logb(a)
    // log(n) is natural log
    std::size_t maxDegree{
        static_cast<std::size_t>(std::log(_numNodes) / std::log(2))};
    std::vector<Node*> trees(maxDegree + 1, nullptr);

    while (true) {
      if (trees[static_cast<std::size_t>(node->_degree)]) {
        Node* t{trees[static_cast<std::size_t>(node->_degree)]};

        if (t == node) {
          break;
        }

        trees[static_cast<std::size_t>(node->_degree)] = nullptr;

        // break t link
        t->_right->_left = t->_left;
        t->_left->_right = t->_right;
        // we merge t with node because these 2 nodes have the same degree
        if (compFn(node->_key, t->_key)) {
          // node is still smaller than t, add t as child of node
          // move t to child of node
          _add_child(node, t);
        } else {
          // add node as child of t
          if (node->is_single_child()) {
            // when there are only 2 nodes: node and t, when we merge t with
            // node only 1 node remains
            // set circular link since only t exists after merge
            t->_right = t;
            t->_left = t;
            _add_child(t, node);
            node = t;
          } else {
            // link t with node sibling, node will be merged into t, t replaces
            // node
            node->_left->_right = t;
            node->_right->_left = t;
            t->_right = node->_right;
            t->_left = node->_left;
            _add_child(t, node);
            node = t;
          }
        }
        continue;
      } else {
        trees[static_cast<std::size_t>(node->_degree)] = node;
      }
      node = node->_right;
    }
    return node;
  }

  template <concepts::IsSameBase<Key> K>
  Node* _update_key(Node* heap, Node* node, K&& newKey) {
    if (compFn(node->_key, newKey)) {
      return heap;
    }
    node->_key = std::forward<K>(newKey);
    Node* parent{node->_parent};
    if (parent && compFn(node->_key, parent->_key)) {
      heap = _cut(heap, node);
      heap = _cascade_cut(heap, parent);
    } else {
      if (compFn(node->_key, heap->_key)) {
        heap = node;
      }
    }
    return heap;
  }

  // remove node from current tree
  // add node to heap
  Node* _cut(Node* heap, Node* node) {
    if (node->is_single_child()) {
      node->_parent->_child = nullptr;
    } else {
      node->_right->_left = node->_left;
      node->_left->_right = node->_right;
      node->_parent->_child = node->_right;
    }
    node->_right = node->_left = node;
    node->_isMarked = false;
    --node->_parent->_degree;
    node->_parent = nullptr;
    return _merge(heap, node);
  }

  Node* _cascade_cut(Node* heap, Node* node) {
    Node* parent{node->_parent};
    if (parent) {
      if (!node->_isMarked) {
        node->_isMarked = true;
      } else {
        heap = _cut(heap, node);
        heap = _cascade_cut(heap, parent);
      }
    }
    return heap;
  }

  void _delete_all(Node* root) {
    if (!root) {
      return;
    }

    Node* node{root};
    do {
      _delete_all(node->_child);
      Node* temp{node};
      node = temp->_right;
      delete temp;
    } while (node != root);
    return;
  }

  template <concepts::IsSameBase<Key> K, concepts::IsSameBase<Value> V>
  Node* _insert(K&& key, V&& value) {
    Node* newNode{_create_node(std::forward<K>(key), std::forward<V>(value))};
    ++_numNodes;
    _minNode = _merge(_minNode, newNode);
    return _minNode;
  }

  Node* _merge(Node* root1, Node* root2) {
    if (!root1) {
      return root2;
    }
    if (!root2) {
      return root1;
    }
    Node* root1Right{root1->_right};
    Node* root2Left{root2->_left};
    root1->_right = root2;
    root2->_left = root1;
    root1Right->_left = root2Left;
    root2Left->_right = root1Right;
    return compFn(root1->_key, root2->_key) ? root1 : root2;
  }

  void _add_child(Node* parent, Node* child) {
    // break child linkage with other nodes
    child->_left = child;
    child->_right = child;

    child->_parent = parent;
    ++parent->_degree;
    parent->_child = _merge(parent->_child, child);
  }

  void _unmark_and_unparent_all(Node* root) {
    if (!root) {
      return;
    }
    Node* node{root};
    do {
      node->_isMarked = false;
      node->_parent = nullptr;
      node = node->_right;
    } while (node != root);
  }

  template <concepts::IsSameBase<Key> K, concepts::IsSameBase<Value> V>
  Node* _create_node(K&& key, V&& value) const {
    return new Node{std::forward<K>(key), std::forward<V>(value)};
  }

  void _print_tree(std::ostream& os, Node* node) const {
    if (!node) {
      return;
    }
    Queue<Node*> queue{};
    queue.push_back(node);
    int rowNodeCount{1};
    int nextRowNodeCount{};
    while (queue.size()) {
      Node* start{queue.pop_front()};
      node = start;
      if (node->has_parent()) {
        os << "[";
      }
      do {
        os << "(" << node->_key << ")";
        if (!node->is_single_child() && node->_right != start) {
          os << " ----> ";
          ++rowNodeCount;
        }

        if (node->has_children()) {
          queue.push_back(node->_child);
          ++nextRowNodeCount;
        }

        node = node->_right;
        --rowNodeCount;
      } while (node != start);
      if (start->has_parent()) {
        os << "](" << start->_parent->_key << ")     ";
      }
      if (rowNodeCount == 0) {
        os << "\n";
        rowNodeCount = nextRowNodeCount;
        nextRowNodeCount = 0;
      }
    }
  }
};
