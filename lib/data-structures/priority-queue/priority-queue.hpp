#pragma once

#include <cmath>
#include <concept.hpp>
#include <cstddef>
#include <heap.hpp>
#include <helpers.hpp>
#include <initializer_list>
#include <iterator>
#include <string>
#include <utility>

#define PRIORITY_QUEUE_DEBUG 0

#if PRIORITY_QUEUE_DEBUG == 1
#define PRIORITY_QUEUE_DEBUG_MS(mes)                                           \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define PRIORITY_QUEUE_DEBUG_MS(mes)                                           \
  do {                                                                         \
  } while (0)
#endif

template <typename T, typename Comparator = std::less_equal<T>>
class PriorityQueue {

private:
  Heap<T, Comparator> _container{};

public:
  using value_type = T;
  using pointer = value_type*;
  using reference = value_type&;
  using rvalue_reference = value_type&&;
  using const_reference = const value_type&;
  using self = PriorityQueue<T, Comparator>;

  PriorityQueue() = default;

  PriorityQueue(std::initializer_list<T> list) : _container(list) {
    PRIORITY_QUEUE_DEBUG_MS("PRIORITY_QUEUE List Ctor");
  };

  template <std::random_access_iterator Iterator>
  PriorityQueue(Iterator begin, Iterator end) : _container(begin, end) {
    PRIORITY_QUEUE_DEBUG_MS("PRIORITY_QUEUE Iterator Ctor");
  }

  ~PriorityQueue() { PRIORITY_QUEUE_DEBUG_MS("PRIORITY_QUEUE Dtor"); };

  PriorityQueue(const self& other) : _container{other._container} {
    PRIORITY_QUEUE_DEBUG_MS("PRIORITY_QUEUE Copy Ctor");
  };

  PriorityQueue(self&& other) noexcept
      : _container{std::move(other._container)} {
    PRIORITY_QUEUE_DEBUG_MS("PRIORITY_QUEUE Move Ctor");
  };

  self& operator=(const self& other) {
    PRIORITY_QUEUE_DEBUG_MS("PRIORITY_QUEUE Copy Operator");
    self copy{other};
    copy.swap(*this);
    return *this;
  };

  self& operator=(self&& other) noexcept {
    PRIORITY_QUEUE_DEBUG_MS("PRIORITY_QUEUE Move Operator");
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

  reference top() noexcept { return _container.top(); }

  void push(const_reference data) { _container.push(data); }

  void push(rvalue_reference data) { _container.push(std::move(data)); }

  value_type pop() { return _container.pop(); }
};