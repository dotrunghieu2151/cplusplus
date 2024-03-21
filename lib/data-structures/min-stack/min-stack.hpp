#pragma once

#include <cmath>
#include <cstddef>
#include <heap.hpp>
#include <helpers.hpp>
#include <initializer_list>
#include <iterator>
#include <string>
#include <utility>
#include <vector.hpp>

#define MIN_STACK_DEBUG 0

#if MIN_STACK_DEBUG == 1
#define MIN_STACK_DEBUG_MS(mes)                                                \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define MIN_STACK_DEBUG_MS(mes)                                                \
  do {                                                                         \
  } while (0)
#endif

template <typename T, typename Comparator = std::less_equal<T>> class MinStack {

private:
  Vector<T> _stack{};
  Vector<T> _minStack{};

public:
  using value_type = T;
  using pointer = value_type*;
  using reference = value_type&;
  using rvalue_reference = value_type&&;
  using const_reference = const value_type&;
  using self = MinStack<T, Comparator>;

  MinStack() = default;

  // MinStack(std::initializer_list<T> list) : _stack(list) {
  //   MIN_STACK_DEBUG_MS("MINSTACK List Ctor");
  // };

  // template <std::random_access_iterator Iterator>
  // MinStack(Iterator begin, Iterator end) : _stack(begin, end) {
  //   MIN_STACK_DEBUG_MS("MINSTACK Iterator Ctor");
  // }

  ~MinStack() { MIN_STACK_DEBUG_MS("MINSTACK Dtor"); };

  MinStack(const self& other)
      : _stack{other._stack}, _minStack{other._minStack} {
    MIN_STACK_DEBUG_MS("MINSTACK Copy Ctor");
  };

  MinStack(self&& other) noexcept
      : _stack{std::move(other._stack)}, _minStack{std::move(other._minStack)} {
    MIN_STACK_DEBUG_MS("MINSTACK Move Ctor");
  };

  self& operator=(const self& other) {
    MIN_STACK_DEBUG_MS("MINSTACK Copy Operator");
    self copy{other};
    copy.swap(*this);
    return *this;
  };

  self& operator=(self&& other) noexcept {
    MIN_STACK_DEBUG_MS("MINSTACK Move Operator");
    self move{std::move(other)};
    move.swap(*this);
    return *this;
  };

  void swap(self& other) noexcept {
    using std::swap;
    swap(_stack, other._stack);
    swap(_minStack, other._minStack);
  }

  void friend swap(self& e1, self& e2) noexcept { e1.swap(e2); };

  bool empty() const noexcept { return _stack.empty(); }

  reference top() noexcept { return _stack.back(); }

  reference getMin() noexcept { return _minStack.back(); }

  void push(const_reference data) {
    _stack.push_back(data);
    T min{(_minStack.size() && _minStack.back() < data ? _minStack.back()
                                                       : data)};
    _minStack.push_back(std::move(min));
  }

  void push(rvalue_reference data) {
    _stack.push_back(std::move(data));
    T min{(_minStack.size() && _minStack.back() < _stack.back()
               ? _minStack.back()
               : _stack.back())};
    _minStack.push_back(std::move(min));
  }

  void pop() {
    _minStack.pop_back();
    _stack.pop_back();
  }
};