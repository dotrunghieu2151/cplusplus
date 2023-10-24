#pragma once

#include <algorithm>
#include <circular-buffer.hpp>
#include <cmath>
#include <concept.hpp>
#include <cstddef>
#include <initializer_list>
#include <simple-deque.hpp>
#include <string>
#include <utility>

template <typename T> class Deque {
  inline static constexpr std::size_t DEFAULT_DEQUE_BUFFER_SIZE{4096};
  inline static constexpr std::size_t DEFAULT_DEQUE_MAP_MIN_SIZE{8};

public:
  class Iterator;
  class ReverseIterator;

  using iterator = Iterator;
  using reverse_iterator = ReverseIterator;
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value = T;
  using pointer = value*;
  using reference = value&;
  using rvalue_reference = value&&;
  using const_reference = const value&;
  using self = Deque<T>;

private:
  // inline constexpr static std::size_t _chunk_size{
  //     sizeof(T) > DEFAULT_DEQUE_BUFFER_SIZE
  //         ? 16
  //         : DEFAULT_DEQUE_BUFFER_SIZE / sizeof(T)};

  inline constexpr static std::size_t _chunk_size{5};

  using buffer = CircularBuffer<T, _chunk_size>;
  using map = SimpleDeque<buffer>;

  map _map;
  std::size_t _size{};
  iterator _head{};
  iterator _tail{};

public:
  /* Constructors */
  Deque() = delete;
  Deque(std::size_t count)
      : _map{std::max(get_blocks_count(count), DEFAULT_DEQUE_MAP_MIN_SIZE)} {
    std::size_t blockCount{get_blocks_count(count)};
    typename map::iterator mapMiddleIterator{_map.begin()};
    for (std::size_t i{}; i < blockCount; ++i) {
      _map.push_back(buffer());
    }
    _head = iterator{mapMiddleIterator, mapMiddleIterator->begin(), this};
    _tail = _head;
  };

  Deque(std::initializer_list<value> list) : Deque(list.size()) {
    for (const_reference ele : list) {
      push_back(ele);
    }
  };

  ~Deque() = default;

  Deque(const self& copy) : Deque(copy.size()) {
    for (const_reference i : copy) {
      push_back(i);
    }
  }

  self& operator=(const self& copy) {
    self tmp{copy};
    tmp.swap(*this);
    return *this;
  }

  Deque(self&& move) noexcept
      : _map{std::move(move._map)}, _size{move._size}, _head{move._head},
        _tail{move._tail} {
    move._size = 0;
    move._head = iterator{};
    move._tail = iterator{};
    _head._deque_ptr = this;
    _tail._deque_ptr = this;
  };
  self& operator=(self&& move) noexcept {
    self moved{std::move(move)};
    moved.swap(*this);
    return *this;
  };

  /* iterator */
  iterator begin() { return _head; };
  iterator end() { return _tail; };

  iterator begin() const { return _head; };
  iterator end() const { return _tail; };

  iterator cbegin() const { return _head; };
  iterator cend() const { return _tail; };

  /* Capacity */
  bool empty() const noexcept { return _size == 0; };
  bool is_full() const noexcept {
    return _map.capacity() * _chunk_size == _size;
  };
  std::size_t size() const noexcept { return _size; };
  void shrink_to_fit();

  /* Element access */
  reference operator[](std::size_t index) { return *(_head + index); };
  reference operator[](std::size_t index) const { return *(_head + index); };

  reference at(std::size_t index) {
    validateIndex(index);
    return (*this)[index];
  };
  reference at(std::size_t index) const {
    validateIndex(index);
    return (*this)[index];
  };

  reference front() { return *_head; };
  reference front() const { return *_head; };

  reference back() { return *(_tail - 1); };
  reference back() const { return *(_tail - 1); };

  /* modifiers */
  void push_back(const_reference value) {
    if (!_tail._block->is_full()) {
      _tail._block->push_back(value);
      _tail._current = _tail._block->end();
    } else if (++_tail._block != _map.end()) {
      if (!_tail._block->capacity()) {
        // the right has exhausted spare block
        // if map still has spare blocks left, steal that
        // else allocate new buffer
        *(_tail._block) =
            _head._block != _map.begin() && _map.begin()->capacity()
                ? _map.pop_front()
                : buffer();
      }
      _tail._block->push_back(value);
      _tail._current = _tail._block->begin();
    } else {
      // map has exhausted all right spare blocks
      // if there are left free blocks, steal or allocate new buffer
      buffer buf(_head._block != _map.begin() && _map.begin()->capacity()
                     ? _map.pop_front()
                     : buffer());
      _map.push_back(std::move(buf));
      // _tail is invalidated, need to assign new block
      _tail._block = _map.end() - 1;
      _tail._block->push_back(value);
      _tail._current = _tail._block->begin();
    }
    ++_size;
  };
  void push_back(rvalue_reference value) {
    if (!_tail._block->is_full()) {
      _tail._block->push_back(std::move(value));
      _tail._current = _tail._block->end();
    } else if (++_tail._block != _map.end()) {
      if (!_tail._block->capacity()) {
        // the right has exhausted spare block
        // if map still has spare blocks left, steal that
        // else allocate new buffer
        *(_tail._block) =
            _head._block != _map.begin() && _map.begin()->capacity()
                ? _map.pop_front()
                : buffer();
      }
      _tail._block->push_back(std::move(value));
      _tail._current = _tail._block->begin();
    } else {
      // map has exhausted all right spare blocks
      // if there are left free blocks, steal or allocate new buffer
      buffer buf(_head._block != _map.begin() && _map.begin()->capacity()
                     ? _map.pop_front()
                     : buffer());
      _map.push_back(std::move(buf));
      // _tail is invalidated, need to assign new block
      _tail._block = _map.end() - 1;
      _tail._block->push_back(std::move(value));
      _tail._current = _tail._block->begin();
    }
    ++_size;
  };

  void push_front(const_reference value) {
    if (!_head._block->is_full()) {
      _head._block->push_front(value);
      _head._current = _head._block->begin();
      if (_head._block == _tail._block) {
        _tail._current = _tail._block->end();
      }
    } else if (_head._block != _map.begin()) {
      --_head._block;
      if (!_head._block->capacity()) {
        *(_head._block) =
            _tail._block != (_map.end() - 1) && (_map.end() - 1)->capacity()
                ? _map.pop_back()
                : buffer();
      }
      _head._block->push_front(value);
      _head._current = _head._block->end() - 1;
    } else {
      --_head._block;
      buffer buff(_tail._block != (_map.end() - 1) &&
                          (_map.end() - 1)->capacity()
                      ? _map.pop_back()
                      : buffer());
      _map.push_front(std::move(buff));
      _head._block = _map.begin();
      _head._block->push_front(value);
      _head._current = _head._block->end() - 1;
    }
    ++_size;
  };
  void push_front(rvalue_reference value) {
    if (!_head._block->is_full()) {
      _head._block->push_front(std::move(value));
      _head._current = _head._block->begin();
      if (_head._block == _tail._block) {
        _tail._current = _tail._block->end();
      }
    } else if (_head._block != _map.begin()) {
      --_head._block;
      if (!_head._block->capacity()) {
        *(_head._block) =
            _tail._block != (_map.end() - 1) && (_map.end() - 1)->capacity()
                ? _map.pop_back()
                : buffer();
      }
      _head._block->push_front(std::move(value));
      _head._current = _head._block->end() - 1;
    } else {
      --_head._block;
      buffer buff(_tail._block != (_map.end() - 1) &&
                          (_map.end() - 1)->capacity()
                      ? _map.pop_back()
                      : buffer());
      _map.push_front(std::move(buff));
      _head._block = _map.begin();
      _head._block->push_front(std::move(value));
      _head._current = _head._block->end() - 1;
    }
    ++_size;
  };

  value pop_front() {
    if (!_head._block->empty()) {
      value ele{_head._block->pop_front()};
      if (_head._block->empty()) {
        ++_head._block;
      }
      _head._current = _head._block->begin();
      --_size;
      return ele;
    } else {
      ++_head._block;
      return pop_front();
    }
  };
  value pop_back() {
    if (!_tail._block->empty()) {
      value ele{_tail._block->pop_back()};
      _tail._current = _tail._block->end();
      --_size;
      return ele;
    } else {
      --_tail._block;
      return pop_back();
    }
  };

  iterator insert(iterator pos, const_reference value);
  iterator insert(iterator pos, rvalue_reference value);

  template <typename InputIter>
  iterator insert(iterator pos, InputIter begin, InputIter end);

  iterator erase(iterator pos);
  template <typename InputIter> iterator erase(iterator begin, iterator end);

  void clear();
  void resize();

  void swap(self& other) noexcept {
    using std::swap;
    swap(_size, other._size);
    swap(_map, other._map);
    swap(_head, other._head);
    swap(_tail, other._tail);
  }

  void friend swap(self& a, self& b) noexcept { a.swap(b); };

  // helpers
private:
  std::size_t get_blocks_count(std::size_t size) const {
    return std::max<std::size_t>(
        std::round(size / static_cast<double>(_chunk_size)), 1);
  };

  void validateIndex(std::size_t index) {
    if (index >= size()) {
      throw OutOfRangeException{"Index out of range"};
    }
  }

public:
  class OutOfRangeException : public std::exception {
  private:
    std::string message;

  public:
    OutOfRangeException(std::string msg) : message{msg} {}
    std::string what() { return message; }
  };

  class Iterator {
    friend class Deque;

  public:
    using iterator_category = iterator_category;
    using difference_type = difference_type;

  public:
    Iterator() = default;

  private:
    typename map::iterator _block{};
    typename buffer::iterator _current{};
    Deque<T>* _deque_ptr{};

    Iterator(typename map::iterator block, typename buffer::iterator current,
             Deque<T>* deque_ptr)
        : _block{block}, _current{current}, _deque_ptr{deque_ptr} {};

  public:
    reference operator*() { return *_current; };
    pointer operator->() { return &(*_current); };

    Iterator& operator++() {
      ++_current;
      if (_current == _block->end() && _block != _deque_ptr->end()._block) {
        // jump to the next block
        ++_block;
        _current = _block->begin();
      }
      return *this;
    }

    Iterator operator++(int) const {
      Iterator tmp{*this};
      ++(*this);
      return tmp;
    }

    Iterator& operator+=(int index) {
      typename buffer::iterator::difference_type offset(_block->capacity() -
                                                        _block->size() + index);
      if (offset >= _chunk_size) {
        typename map::iterator::difference_type blockOffset(offset /
                                                            _chunk_size);
        typename buffer::iterator::difference_type currentOffset(
            offset - blockOffset * _chunk_size);
        _block += blockOffset;
        _current = _block->begin() + currentOffset;
      } else {
        _current += index;
      }
      return *this;
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

    Iterator& operator--() {
      if (_current == _block->begin()) {
        --_block;
        _current = _block->end() - 1;
      } else {
        --_current;
      }
      return *this;
    }

    Iterator operator--(int) const {
      Iterator tmp{*this};
      --(*this);
      return tmp;
    }

    Iterator& operator-=(int index) {
      typename buffer::iterator::difference_type offset(_block->capacity() -
                                                        _block->size() + index);
      if (offset >= _chunk_size) {
        typename map::iterator::difference_type blockOffset(offset /
                                                            _chunk_size);
        typename buffer::iterator::difference_type currentOffset(
            offset - blockOffset * _chunk_size);
        _block -= blockOffset;
        _current = _block->end() - currentOffset;
      } else {
        _current -= index;
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

    difference_type operator-(const Iterator& other) const {
      switch (true) {
      case (*this) > other: {
        std::size_t blockDiff = (_block - other._block) * _chunk_size;
        return blockDiff - (other._current - other._block->begin()) +
               (_current - _block->begin());
        break;
      }

      case (*this) < other: {
        std::size_t blockDiff = (other._block - _block) * _chunk_size;
        return blockDiff - (_current - _block->begin()) +
               (other._current - other._block->begin());
        break;
      }

      default:
        return 0;
      }
    }

    bool operator==(const Iterator& other) const {
      return _block == other._block && _current == other._current;
    }

    bool operator!=(const Iterator& other) const { return !(*this == other); }

    friend bool operator<(const Iterator& i1, const Iterator& i2) {
      return i1._block == i2._block ? i1._current < i2._current
                                    : i1._block < i2._block;
    };
    friend bool operator>(const Iterator& i1, const Iterator& i2) {
      return operator<(i2, i1);
    };

    friend bool operator<=(const Iterator& i1, const Iterator& i2) {
      return !(operator>(i1, i2));
    };
    friend bool operator>=(const Iterator& i1, const Iterator& i2) {
      return !(operator<(i1, i2));
    };

    reference operator[](difference_type index) const {
      (*this) += index;
      return *(*this);
    }
  };
};