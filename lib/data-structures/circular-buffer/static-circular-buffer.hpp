#pragma once

#include <array.hpp>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector.hpp>

#define STATIC_CIRCULAR_BUFFER_DEBUG 0

#if STATIC_CIRCULAR_BUFFER_DEBUG == 1
#define STATIC_CIRCULAR_BUFFER_DEBUG_MS(mes)                                   \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define STATIC_CIRCULAR_BUFFER_DEBUG_MS(mes)                                   \
  do {                                                                         \
  } while (0)
#endif

template <typename T, std::size_t N> class StaticCircularBuffer {
public:
  template <bool IsConst> class Iterator;

  using iterator = Iterator<false>;
  using const_iterator = Iterator<true>;
  using pointer = T*;
  using reference = T&;
  using rvalue_reference = T&&;
  using const_reference = const T&;
  using value_type = T;
  using self = StaticCircularBuffer<T, N>;
  using difference_type = typename iterator::difference_type;

private:
  std::size_t _head{};
  std::size_t _tail{};
  std::size_t _size{};
  std::array<T, N> _elements{};

public:
  StaticCircularBuffer() = default;
  StaticCircularBuffer(const_reference default_value) : StaticCircularBuffer() {
    _elements.fill(default_value);
    _head = 0;
    _tail = 0;
    _size = N;
    STATIC_CIRCULAR_BUFFER_DEBUG_MS("StaticCircularBuffer Ctor default value");
  };

  StaticCircularBuffer(rvalue_reference default_value)
      : StaticCircularBuffer() {
    _elements.fill(std::move(default_value));
    _head = 0;
    _tail = 0;
    _size = N;
    STATIC_CIRCULAR_BUFFER_DEBUG_MS("StaticCircularBuffer Ctor default value");
  };

  ~StaticCircularBuffer() {
    STATIC_CIRCULAR_BUFFER_DEBUG_MS("StaticCircularBuffer Dtor");
  };
  StaticCircularBuffer(const self& other)
      : _head{other._head}, _tail{other._head}, _size{other._size},
        _elements{other._elements} {
    STATIC_CIRCULAR_BUFFER_DEBUG_MS("StaticCircularBuffer Copy Ctor");
  };
  StaticCircularBuffer(self&& other)
      : _head{other._head}, _tail{other._tail}, _size{other._size},
        _elements{std::move(other._elements)} {
    other._head = 0;
    other._tail = 0;
    other._size = 0;
    STATIC_CIRCULAR_BUFFER_DEBUG_MS("StaticCircularBuffer Move Ctor");
  };

  self& operator=(const self& other) {
    STATIC_CIRCULAR_BUFFER_DEBUG_MS("StaticCircularBuffer Copy Operator");
    self copy{other};
    copy.swap(*this);
    return *this;
  };
  self& operator=(self&& other) {
    STATIC_CIRCULAR_BUFFER_DEBUG_MS("StaticCircularBuffer Move Operator");
    self moved{std::move(other)};
    moved.swap(*this);
    return *this;
  };

  const_reference operator[](std::size_t index) const {
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
    swap(_size, other._size);
  }
  friend void swap(self& e1, self& e2) { e1.swap(e2); };

  bool empty() const noexcept { return size() == 0; };
  constexpr ::size_t capacity() const noexcept { return N; };
  std::size_t size() const noexcept { return _size; };
  bool is_full() const noexcept { return _size == N; }

  void push_back(const_reference element) {
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    _elements[_tail] = element;
    _tail = ++_tail == N ? 0 : _tail;
    ++_size;
  }

  void push_back(rvalue_reference element) {
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    _elements[_tail] = std::move(element);
    _tail = ++_tail == N ? 0 : _tail;
    ++_size;
  }

  iterator insert(iterator pos, const_reference element) {
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    ++_size;
    _tail = ++_tail == N ? 0 : _tail;
    for (iterator i{end()}; i != pos; --i) {
      (*i) = *(i - 1);
    }
    (*this)[pos._current] = element;
    return {pos._current, this, pos._steps};
  }

  iterator insert(iterator pos, rvalue_reference element) {
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    ++_size;
    _tail = ++_tail == N ? 0 : _tail;
    for (iterator i{end()}; i != pos; --i) {
      (*i) = std::move(*(i - 1));
    }
    (*this)[pos._current] = std::move(element);
    return {pos._current, this, pos._steps};
  }

  iterator insert(std::size_t index, const_reference element) {
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    ++_size;
    _tail = ++_tail == N ? 0 : _tail;
    for (std::size_t i{_size - 1}; i > index; --i) {
      (*this)[i] = std::move((*this)[i - 1]);
    }
    (*this)[index] = element;
    return {index, this, index};
  }

  iterator insert(std::size_t index, rvalue_reference element) {
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    ++_size;
    _tail = ++_tail == N ? 0 : _tail;
    for (std::size_t i{_size - 1}; i > index; --i) {
      (*this)[i] = std::move((*this)[i - 1]);
    }
    (*this)[index] = std::move(element);
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
      (*this)[i] = std::move((*this)[i - distance]);
    }
    for (std::size_t i{}; i < distance; ++i) {
      (*this)[index + i] = *(start + static_cast<difference_type>(i));
    }
    return {index, this, index};
  }

  iterator erase(iterator start, iterator end) {
    std::size_t distance{static_cast<std::size_t>(std::distance(start, end))};
    for (iterator i{end}; i != this->end(); ++i) {
      *(i - distance) = std::move(*i);
    }
    _tail = distance > _tail ? N - (distance - _tail) : _tail - distance;
    _size -= distance;
    return {start._current, this, start._steps};
  }

  iterator erase(iterator pos) {
    for (iterator i{pos}; i != end(); ++i) {
      (*i) = std::move(*(i + 1));
    }
    _tail = _tail == 0 ? N - 1 : _tail - 1;
    --_size;
    return {pos._current, this, pos._steps};
  }

  iterator erase(std::size_t index) {
    for (std::size_t i{index}; i < _size - 1; ++i) {
      (*this)[i] = std::move((*this)[i + 1]);
    }
    _tail = _tail == 0 ? N - 1 : _tail - 1;
    --_size;
    return {index, this, index};
  }

  void push_front(const_reference element) {
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    _head = _head == 0 ? N - 1 : _head - 1;
    _elements[_head] = element;
    ++_size;
  }

  void push_front(rvalue_reference element) {
    if (is_full()) {
      throw std::runtime_error("buffer is full");
    }
    _head = _head == 0 ? N - 1 : _head - 1;
    _elements[_head] = std::move(element);
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

  const_iterator begin() const { return {_head, this}; }
  const_iterator end() const { return {_tail, this, _size}; }

  const_iterator cbegin() const { return {_head, this}; }
  const_iterator cend() const { return {_tail, this, _size}; }

private:
  void validateIndex(std::size_t index) {
    if (_size > 0 && index >= _size) {
      throw OutOfRangeException{"Index out of range"};
    }
  }

public:
  class OutOfRangeException : public std::exception {
    friend class StaticCircularBuffer;

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

  template <bool IsConst> class Iterator {
    friend class StaticCircularBuffer;

  private:
    using BufferPtr = std::conditional_t<IsConst, const StaticCircularBuffer*,
                                         StaticCircularBuffer*>;
    std::size_t _current{};
    std::size_t _steps{};

    BufferPtr _buffer{};

    Iterator(std::size_t current, BufferPtr buffer, std::size_t steps = 0)
        : _current{current}, _steps{steps}, _buffer{buffer} {};

  public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using self = Iterator<IsConst>;

    Iterator() = default;

    template <bool Q = IsConst>
    typename std::enable_if<Q, const_reference>::type operator*() const {
      return _buffer->_elements[_current];
    }

    template <bool Q = IsConst>
    typename std::enable_if<!Q, reference>::type operator*() const {
      return _buffer->_elements[_current];
    }

    pointer operator->() const { return &_buffer->_elements[_current]; };

    self& operator++() {
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

    self operator++(int) {
      self tmp{*this};
      ++(*this);
      return tmp;
    }

    self& operator--() {
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

    self operator--(int) {
      self tmp{*this};
      --(*this);
      return tmp;
    }

    self operator+(std::size_t index) const {
      self tmp(*this);
      tmp += index;
      return tmp;
    }

    friend self operator+(const difference_type index, const self& other) {
      return other + index;
    }

    self& operator+=(std::size_t distance) {
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

    self operator-(std::size_t index) const {
      self tmp{*this};
      tmp -= index;
      return tmp;
    }

    friend self operator-(std::size_t index, const self& other) {
      return other - index;
    }

    difference_type operator-(const self& other) const {
      return static_cast<difference_type>(_steps) -
             static_cast<difference_type>(other._steps);
    }

    self& operator-=(std::size_t distance) {
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

    bool operator==(const self& other) const {
      return _current == other._current && _steps == other._steps;
    }

    bool operator!=(const self& other) const { return !(*this == other); }

    // for random access
    reference operator[](difference_type index) const { return *this + index; }
  };
};
