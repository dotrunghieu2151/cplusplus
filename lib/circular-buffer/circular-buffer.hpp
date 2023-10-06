#pragma once

#include <allocator.hpp>
#include <array.hpp>
#include <concept.hpp>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector.hpp>

template <typename T, std::size_t N, typename Allocator = Allocator<T>>
class CircularBuffer {
public:
  class Iterator;
  class ReverseIterator;

  using iterator = Iterator;
  using reverse_iterator = ReverseIterator;
  using pointer = T*;
  using reference = T&;
  using rvalue_reference = T&&;
  using const_reference = const T&;
  using value = T;
  using self = CircularBuffer<T, N, Allocator>;

public:
  CircularBuffer() : _elements{_allocator.allocate(N)} {
    helpers::printf("CircularBuffer Ctor default");
  };
  CircularBuffer(const_reference default_value) : CircularBuffer() {
    for (std::size_t i{0}; i < N; ++i) {
      _allocator.construct(_elements + i, default_value);
    }
    _head = 0;
    _tail = N;
    helpers::printf("CircularBuffer Ctor default value");
  };

  ~CircularBuffer() {
    helpers::printf("CircularBuffer Dtor");
    while (!empty()) {
      pop_front();
    }
    _allocator.deallocate(_elements);
  };
  CircularBuffer(const self& other)
      : _allocator{other._allocator}, _head{other._head}, _tail{other._head},
        _elements{_allocator.allocate(N)} {
    for (const_reference i : other) {
      push_back(i);
    }
    helpers::printf("CircularBuffer Copy Ctor");
  };
  CircularBuffer(self&& other)
      : _allocator{std::move(other._allocator)}, _head{other._head},
        _tail{other._tail}, _elements{other._elements} {
    helpers::printf("CircularBuffer Move Ctor");
    other._elements = nullptr;
    other._head = 0;
    other._tail = 0;
  };

  self& operator=(const self& other) {
    helpers::printf("CircularBuffer Copy Operator");
    self copy{other};
    copy.swap(*this);
    return *this;
  };
  self& operator=(self&& other) {
    helpers::printf("CircularBuffer Move Operator");
    self moved{std::move(other)};
    moved.swap(*this);
    return *this;
  };

  reference operator[](std::size_t index) const { return _elements[index]; }

  void swap(self& other) noexcept {
    using std::swap;
    swap(_elements, other._elements);
    swap(_head, other._head);
    swap(_tail, other._tail);
    swap(_allocator, other._allocator);
  }
  friend void swap(self& e1, self& e2) { e1.swap(e2); };

  bool empty() const noexcept { return size() == 0; };
  std::size_t capacity() const noexcept { return N; };
  std::size_t size() const noexcept {
    return _tail >= _head ? _tail - _head : N - (_head - _tail);
  };
  bool is_full() const noexcept {
    return _tail == N ? _head == 0 : _tail + 1 == _head;
  }

  void push_back(const_reference element) {
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    if (_tail == N) {
      _allocator.construct(_elements, element);
      _tail = 1;
    } else {
      _allocator.construct(_elements + _tail, element);
      ++_tail;
    }
  }

  void push_back(rvalue_reference element) {
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    if (_tail == N) {
      _allocator.construct(_elements, std::move(element));
      _tail = 1;
    } else {
      _allocator.construct(_elements + _tail, std::move(element));
      ++_tail;
    }
  }

  T pop_front() {
    T value{std::move(_elements[_head])};
    _head = ++_head == N ? 0 : _head;
    _tail = _head == 0 && _tail == N ? 0 : _tail;
    return value;
  }

  iterator begin() { return {_head, *this}; }
  iterator end() { return {_tail, *this, 1}; }

  iterator begin() const { return {_head, *this}; }
  iterator end() const { return {_tail, *this, 1}; }

  iterator cbegin() const { return {_head, *this}; }
  iterator cend() const { return {_tail, *this, 1}; }

private:
  Allocator _allocator{};
  std::size_t _head{};
  std::size_t _tail{};
  pointer _elements{nullptr};

public:
  class BufferFullException : public std::exception {
  private:
    std::string message;

  public:
    BufferFullException(std::string msg) : message{msg} {}
    std::string what() { return message; }
  };

  class Iterator {
    friend class CircularBuffer;

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;

    reference operator*() const { return _buffer[_current]; };
    pointer operator->() const { return &_buffer[_current]; };

    Iterator& operator++() {
      ++_current;
      if (_current == N) {
        _current = 0;
        ++_cycle;
      }
      return *this;
    }

    Iterator operator++(int) {
      Iterator tmp{*this};
      ++(*this);
      return tmp;
    }

    Iterator& operator--() {
      if (_current == 0) {
        _current = N - 1;
        --_cycle;
      } else {
        --_current;
      }
      return *this;
    }

    Iterator operator--(int) {
      Iterator tmp{*this};
      --(*this);
      return tmp;
    }

    Iterator operator+(const difference_type index) const {
      if (_current + index >= N) {
        return {_current + index - N, _buffer, _cycle + 1};
      }
      return {_current + index, _buffer, _cycle};
    }

    friend Iterator operator+(const difference_type index,
                              const Iterator& other) {
      return other + index;
    }

    Iterator& operator+=(const difference_type index) {
      _current =
          _current + index >= N ? _current + index - N : _current + index;
      return *this;
    }

    Iterator operator-(const difference_type index) const {
      if (_current <= index) {
        return {N - (index - _current), _buffer, _cycle - 1};
      }
      return {_current - index, _buffer, _cycle};
    }

    friend Iterator operator-(const difference_type index,
                              const Iterator& other) {
      return other - index;
    }

    Iterator& operator-=(const difference_type index) {
      _current = _current <= index ? N - (index - _current) : _current - index;
      return *this;
    }

    bool operator==(const Iterator& other) const {
      return _current == (_cycle == other._cycle && other._current == N
                              ? 0
                              : other._current);
    }

    bool operator!=(const Iterator& other) const { return !(*this == other); }

    // for random access
    reference operator[](difference_type index) const {
      return *(*this + index);
    }

  private:
    std::size_t _current{};
    int _cycle{};
    const CircularBuffer& _buffer;

    Iterator() = default;
    Iterator(std::size_t current, const CircularBuffer& buffer, int cycle = 0)
        : _current{current}, _cycle{cycle}, _buffer{buffer} {};
  };
};