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
  using value_type = T;
  using self = CircularBuffer<T, N, Allocator>;
  using difference_type = typename iterator::difference_type;

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
    return _elements[get_offset_index(index)];
  }

  reference operator[](std::size_t index) {
    return _elements[get_offset_index(index)];
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

  iterator insert(iterator pos, const_reference element) {
    return insert(pos._current, element);
  }

  iterator insert(iterator pos, rvalue_reference element) {
    return insert(pos._current, std::move(element));
  }

  iterator insert(std::size_t index, const_reference element) {
    using std::swap;
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    ++_size;
    _allocator.construct(_elements + _tail, element);
    _tail = ++_tail == N ? 0 : _tail;
    for (std::size_t i{_size - 1}; i > index; --i) {
      swap((*this)[i], (*this)[i - 1]);
    }
    return {index, this, index};
  }

  iterator insert(std::size_t index, rvalue_reference element) {
    using std::swap;
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    ++_size;
    _allocator.construct(_elements + _tail, std::move(element));
    _tail = ++_tail == N ? 0 : _tail;
    for (std::size_t i{_size - 1}; i > index; --i) {
      swap((*this)[i], (*this)[i - 1]);
    }
    return {index, this, index};
  }

  template <typename Iter>
  iterator insert(std::size_t index, Iter start, Iter end) {
    std::size_t distance{static_cast<std::size_t>(std::distance(start, end))};
    if (size() + distance > capacity()) {
      throw std::runtime_error("buffer is full");
    }
    _size += distance;
    _tail = _tail + distance >= N ? _tail + distance - N : _tail + distance;
    for (std::size_t i{_size - 1}; i >= index + distance; --i) {
      _allocator.construct(_elements + get_offset_index(i),
                           std::move((*this)[i - distance]));
    }
    for (std::size_t i{}; i < distance; ++i) {
      _allocator.construct(_elements + get_offset_index(index + i),
                           *(start + static_cast<difference_type>(i)));
    }
    return {index, this, index};
  }

  iterator erase(iterator start, iterator end) {
    std::size_t distance{static_cast<std::size_t>(std::distance(start, end))};
    for (iterator i{start}; i != end; ++i) {
      (*i).~T();
      (*i) = std::move(*(start + distance));
    }
    _tail = distance > _tail ? N - (distance - _tail) : _tail - distance;
    _size -= distance;
    return {start._current, this, start._steps};
  }

  iterator erase(iterator pos) {
    (*pos).~T();
    for (iterator i{pos}; i != end(); ++i) {
      (*i) = std::move(*(i + 1));
    }
    _tail = _tail == 0 ? N - 1 : _tail - 1;
    --_size;
    return {pos._current, this, pos._steps};
  }

  iterator erase(std::size_t index) {
    (*this)[index].~T();
    for (std::size_t i{index}; i < _size - 1; ++i) {
      (*this)[i] = std::move((*this)[i + 1]);
    }
    _tail = _tail == 0 ? N - 1 : _tail - 1;
    --_size;
    return {index, this, index};
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

  void validateIndex(std::size_t index) const {
    if (index >= _size) {
      throw OutOfRangeException{"Index out of range"};
    }
  }

  std::size_t get_offset_index(std::size_t index) const noexcept {
    std::size_t i{_head + index};
    if (i >= N) {
      i -= N;
    }
    return i;
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
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;

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

    Iterator operator+(std::size_t index) const {
      Iterator tmp{*this};
      tmp += index;
      return tmp;
    }

    friend Iterator operator+(std::size_t index, const Iterator& other) {
      return other + index;
    }

    Iterator& operator+=(std::size_t distance) {
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

    Iterator operator-(std::size_t index) const {
      Iterator tmp{*this};
      tmp -= index;
      return tmp;
    }

    friend Iterator operator-(const difference_type index,
                              const Iterator& other) {
      return other - index;
    }

    difference_type operator-(const Iterator& other) {
      return static_cast<difference_type>(_current) -
             static_cast<difference_type>(other._current);
    }

    Iterator& operator-=(std::size_t distance) {
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