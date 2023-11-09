#pragma once

#include <allocator.hpp>
#include <array.hpp>
#include <concept.hpp>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector.hpp>

#define CIRCULAR_BUFFER_DEBUG 0

#if CIRCULAR_BUFFER_DEBUG == 1
#define CIRCULAR_BUFFER_DEBUG_MS(mes)                                          \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define CIRCULAR_BUFFER_DEBUG_MS(mes)                                          \
  do {                                                                         \
  } while (0)
#endif

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
    CIRCULAR_BUFFER_DEBUG_MS("CircularBuffer Ctor default");
  };
  CircularBuffer(const_reference default_value) : CircularBuffer() {
    for (std::size_t i{0}; i < N; ++i) {
      _allocator.construct(_elements + i, default_value);
    }
    _head = 0;
    _tail = 0;
    _size = N;
    CIRCULAR_BUFFER_DEBUG_MS("CircularBuffer Ctor default value");
  };

  ~CircularBuffer() {
    CIRCULAR_BUFFER_DEBUG_MS("CircularBuffer Dtor");
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
    CIRCULAR_BUFFER_DEBUG_MS("CircularBuffer Copy Ctor");
  };
  CircularBuffer(self&& other)
      : _allocator{std::move(other._allocator)}, _head{other._head},
        _tail{other._tail}, _elements{other._elements}, _size{other._size} {
    other._elements = nullptr;
    other._head = 0;
    other._tail = 0;
    other._size = 0;
    CIRCULAR_BUFFER_DEBUG_MS("CircularBuffer Move Ctor");
  };

  self& operator=(const self& other) {
    CIRCULAR_BUFFER_DEBUG_MS("CircularBuffer Copy Operator");
    self copy{other};
    copy.swap(*this);
    return *this;
  };
  self& operator=(self&& other) {
    CIRCULAR_BUFFER_DEBUG_MS("CircularBuffer Move Operator");
    self moved{std::move(other)};
    moved.swap(*this);
    return *this;
  };

  reference operator[](std::size_t index) const {
    std::size_t i{_head + index};
    if (i >= N) {
      i -= N;
    }
    return _elements[i];
  }

  reference operator[](std::size_t index) {
    std::size_t i{_head + index};
    if (i >= N) {
      i -= N;
    }
    return _elements[i];
  }

  reference at(std::size_t index) {
    validateIndex(index);
    return (*this)[index];
  }

  reference at(std::size_t index) const {
    validateIndex(index);
    return (*this)[index];
  }

  reference front() { return (*this)[0]; };
  reference front() const { return (*this)[0]; };

  reference back() { return (*this)[size() - 1]; };
  reference back() const { return (*this)[size() - 1]; };

  void swap(self& other) noexcept {
    using std::swap;
    swap(_elements, other._elements);
    swap(_head, other._head);
    swap(_tail, other._tail);
    swap(_allocator, other._allocator);
    swap(_size, other._size);
  }
  friend void swap(self& e1, self& e2) { e1.swap(e2); };

  bool empty() const noexcept { return size() == 0; };
  std::size_t capacity() const noexcept { return _elements ? N : 0; };
  std::size_t size() const noexcept { return _size; };
  bool is_full() const noexcept { return _size == N; }

  void push_back(const_reference element) {
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    _allocator.construct(_elements + _tail, element);
    _tail = ++_tail == N ? 0 : _tail;
    ++_size;
  }

  void push_back(rvalue_reference element) {
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    _allocator.construct(_elements + _tail, std::move(element));
    _tail = ++_tail == N ? 0 : _tail;
    ++_size;
  }

  void push_front(const_reference element) {
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    _head = _head == 0 ? N - 1 : _head - 1;
    _allocator.construct(_elements + _head, element);
    ++_size;
  }

  void push_front(rvalue_reference element) {
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    _head = _head == 0 ? N - 1 : _head - 1;
    _allocator.construct(_elements + _head, std::move(element));
    ++_size;
  }

  T pop_front() {
    T value{std::move(_elements[_head])};
    _head = ++_head == N ? 0 : _head;
    --_size;
    return value;
  }

  T pop_back() {
    _tail = _tail == 0 ? N - 1 : _tail - 1;
    T value{std::move(_elements[_tail])};
    --_size;
    return value;
  }

  iterator begin() { return {_head, this}; }
  iterator end() { return {_tail, this, _size}; }

  iterator begin() const { return {_head, this}; }
  iterator end() const { return {_tail, this, _size}; }

  iterator cbegin() const { return {_head, this}; }
  iterator cend() const { return {_tail, this, _size}; }

private:
  Allocator _allocator{};
  std::size_t _head{};
  std::size_t _tail{};
  pointer _elements{nullptr};
  std::size_t _size{};

  void validateIndex(std::size_t index) {
    if (index >= _size) {
      throw OutOfRangeException{"Index out of range"};
    }
  }

public:
  class OutOfRangeException : public std::exception {
    friend class CircularBuffer;

  private:
    std::string message;
    OutOfRangeException(std::string msg) : message{msg} {}
    std::string what() { return message; }
  };

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

    Iterator() = default;

    reference operator*() const { return _buffer->_elements[_current]; };
    pointer operator->() const { return &_buffer->_elements[_current]; };

    Iterator& operator++() {
      ++_current;
      if (_current == N) {
        _current = 0;
      }
      if (_steps == _buffer->size()) {
        _steps = 0;
      } else {
        ++_steps;
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
      } else {
        --_current;
      }
      if (_steps == 0) {
        _steps = _buffer->size();
      } else {
        --_steps;
      }
      return *this;
    }

    Iterator operator--(int) {
      Iterator tmp{*this};
      --(*this);
      return tmp;
    }

    Iterator operator+(const difference_type index) const {
      Iterator tmp{*this};
      tmp += index;
      return tmp;
    }

    friend Iterator operator+(const difference_type index,
                              const Iterator& other) {
      return other + index;
    }

    Iterator& operator+=(difference_type distance) {
      if (_current + distance >= N) {
        _current = (_current + distance) - N;
      } else {
        _current += distance;
      }
      if (_steps + distance > _buffer->size()) {
        _steps = _steps + distance - _buffer->size();
      } else {
        _steps += distance;
      }
      return *this;
    }

    Iterator operator-(const difference_type index) const {
      Iterator tmp{*this};
      tmp -= index;
      return tmp;
    }

    friend Iterator operator-(const difference_type index,
                              const Iterator& other) {
      return other - index;
    }

    difference_type operator-(const Iterator& other) {
      return _current - other._current;
    }

    Iterator& operator-=(difference_type distance) {
      if (_current < distance) {
        _current = N - (distance - _current);
      } else {
        _current -= distance;
      }
      // convert distance to position
      if (_steps <= distance) {
        _steps = _buffer->size() - distance;
      } else {
        _steps -= distance;
      }
      return *this;
    }

    bool operator==(const Iterator& other) const {
      return _current == other._current && _steps == other._steps;
    }

    bool operator!=(const Iterator& other) const { return !(*this == other); }

    // for random access
    reference operator[](difference_type index) const {
      return (*this) + index;
    }

  private:
    std::size_t _current{};
    std::size_t _steps{};
    const CircularBuffer* _buffer{};

    Iterator(std::size_t current, const CircularBuffer* buffer,
             std::size_t steps = 0)
        : _current{current}, _steps{steps}, _buffer{buffer} {};
  };
};