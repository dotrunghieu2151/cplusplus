#pragma once

#include <allocator.hpp>
#include <cmath>
#include <concept.hpp>
#include <cstddef>
#include <helpers.hpp>
#include <initializer_list>
#include <iterator>
#include <string>
#include <utility>
#include <vector.hpp>

#define HEAP_DEBUG 0

#if HEAP_DEBUG == 1
#define HEAP_DEBUG_MS(mes)                                                     \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define HEAP_DEBUG_MS(mes)                                                     \
  do {                                                                         \
  } while (0)
#endif

template <concepts::Comparable T, typename Comparator = std::less_equal<T>>
class Heap {

private:
  Vector<T> _container{};
  static constexpr Comparator compFn{};

public:
  using value_type = T;
  using pointer = value_type*;
  using reference = value_type&;
  using rvalue_reference = value_type&&;
  using const_reference = const value_type&;
  using self = Heap<T, Comparator>;

  Heap() = default;

  Heap(std::initializer_list<T> list) : _container(list) {
    HEAP_DEBUG_MS("HEAP List Ctor");
    heapify();
  };

  // heapify array-like container, by heapify in reverse order from the last
  // non-leaf node => start of the container
  template <std::random_access_iterator Iterator>
  Heap(Iterator begin, Iterator end) : _container(begin, end) {
    heapify();
  }

  ~Heap() { HEAP_DEBUG_MS("HEAP Dtor"); };

  Heap(const self& other) : _container{other._container} {
    HEAP_DEBUG_MS("HEAP Copy Ctor");
  };

  Heap(self&& other) noexcept : _container{std::move(other._container)} {
    HEAP_DEBUG_MS("HEAP Move Ctor");
  };

  self& operator=(const self& other) {
    HEAP_DEBUG_MS("HEAP Copy Operator");
    self copy{other};
    copy.swap(*this);
    return *this;
  };

  self& operator=(self&& other) noexcept {
    HEAP_DEBUG_MS("HEAP Move Operator");
    self move{std::move(other)};
    move.swap(*this);
    return *this;
  };

  void swap(self& other) noexcept {
    using std::swap;
    swap(_container, other._container);
  }

  void friend swap(self& e1, self& e2) noexcept { e1.swap(e2); };

  bool empty() const noexcept { return _container.empty(); }

  reference top() noexcept { return _container.front(); }

  void push(const_reference data) {
    // first insert at the nd;
    _container.push_back(data);
    // move node to ensure heap property
    move_up(_container.size() - 1);
  }

  void push(rvalue_reference data) {
    // first insert at the nd;
    _container.push_back(std::move(data));
    // move node to ensure heap property
    move_up(_container.size() - 1);
  }

  value_type pop() {
    // swap the top and the last node
    std::swap(_container[0], _container.back());
    // remove the top node (now last ele)
    value_type top{_container.pop_back()};
    // move the last node down to its original position
    move_down(0);
    return top;
  }

  value_type remove(std::size_t index) {
    std::swap(_container[index], _container.back());
    value_type removedEle{_container.pop_back()};
    move_down(index);
    return removedEle;
  }

  void remove(const_reference data) {
    for (std::size_t index{}; index < _container.size(); ++index) {
      if (_container[index] == data) {
        remove(index);
        return;
      }
    }
  }

private:
  std::size_t parent_index(std::size_t index) { return (index - 1) / 2; }
  std::size_t left_child_index(std::size_t index) { return index * 2 + 1; }
  std::size_t right_child_index(std::size_t index) { return index * 2 + 2; }

  void heapify() {
    // find last non-leaf node
    // last non-leaf node = parent of last leaf node (due to breadth-first
    // order) last leaf node = N - 1; => last non-leaf node = ((N -1) -1) / 2 =
    // N/2 -1
    const int lastNonLeafNodeIndex(_container.size() / 2 - 1);
    for (int i{lastNonLeafNodeIndex}; i >= 0; --i) {
      move_down(static_cast<std::size_t>(i));
    }
  }
  void move_up(std::size_t index) {
    std::size_t parentIndex{parent_index(index)};
    while (index != 0 && compFn(_container[index], _container[parentIndex])) {
      std::swap(_container[index], _container[parentIndex]);
      index = parentIndex;
      parentIndex = parent_index(index);
    }
  }

  void move_down(std::size_t index) {
    std::size_t leftChildIndex{left_child_index(index)};
    std::size_t rightChildIndex{right_child_index(index)};
    std::size_t nextIndex{leftChildIndex};
    // compare left and right, get the smaller / greater and swap with parent
    // (index) overflow std::size_t
    if (leftChildIndex >= _container.size()) {
      return;
    }
    if (rightChildIndex < _container.size() &&
        !compFn(_container[leftChildIndex], _container[rightChildIndex])) {
      nextIndex = rightChildIndex;
    }
    if (!compFn(_container[index], _container[nextIndex])) {
      std::swap(_container[index], _container[nextIndex]);
      move_down(nextIndex);
    }
    return;
  }
};

template <typename T> using MinHeap = Heap<T>;
template <typename T> using MaxHeap = Heap<T, std::greater_equal<T>>;