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

#define DEQUE_DEBUG 0

#if DEQUE_DEBUG == 1
#define DEQUE_DEBUG_MS(mes)                                                    \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define DEQUE_DEBUG_MS(mes)                                                    \
  do {                                                                         \
  } while (0)
#endif

template <typename T> class Deque {
  inline static constexpr std::size_t DEFAULT_DEQUE_BUFFER_SIZE{4096};
  inline static constexpr std::size_t DEFAULT_DEQUE_MAP_MIN_SIZE{2};

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
  inline constexpr static std::size_t _chunk_size{
      sizeof(T) > DEFAULT_DEQUE_BUFFER_SIZE
          ? 16
          : DEFAULT_DEQUE_BUFFER_SIZE / sizeof(T)};

  using buffer = CircularBuffer<T, _chunk_size>;
  using map = SimpleDeque<buffer>;

  map _map;
  std::size_t _size{};
  iterator _head{};
  iterator _tail{};

public:
  /* Constructors */
  Deque(std::size_t count = 0)
      : _map{std::max(get_blocks_count(count), DEFAULT_DEQUE_MAP_MIN_SIZE)} {
    std::size_t blockCount{get_blocks_count(count)};
    for (std::size_t i{}; i < blockCount; ++i) {
      _map.push_back(buffer());
    }
    std::size_t blockIndex(blockCount * 0.5 - 1);
    _head = iterator{blockIndex, 0, this};
    _tail = _head;
    DEQUE_DEBUG_MS("Deque Ctor default");
  };

  Deque(std::initializer_list<value> list) : Deque(list.size()) {
    for (const_reference ele : list) {
      push_back(ele);
    }
    DEQUE_DEBUG_MS("Deque List Ctor default");
  };

  ~Deque() = default;

  Deque(const self& copy) : Deque(copy.size()) {
    for (const_reference i : copy) {
      push_back(i);
    }
    DEQUE_DEBUG_MS("Deque Copy Ctor");
  }

  self& operator=(const self& copy) {
    DEQUE_DEBUG_MS("Deque Copy Operator");
    self tmp{copy};
    tmp.swap(*this);
    return *this;
  }

  Deque(self&& move) noexcept
      : _map{std::move(move._map)}, _size{move._size}, _head{move._head},
        _tail{move._tail} {
    DEQUE_DEBUG_MS("Deque Move Ctor");
    move._size = 0;
    move._head = iterator{};
    move._tail = iterator{};
    _head._deque_ptr = this;
    _tail._deque_ptr = this;
  };
  self& operator=(self&& move) noexcept {
    DEQUE_DEBUG_MS("Deque Move Operator");
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
  reference operator[](std::size_t index) {
    std::size_t headSize{_map[_head._blockIndex].size()};
    std::size_t blockBeginIndex{_head._blockIndex};
    if (index >= headSize) {
      index -= headSize;
      ++blockBeginIndex;
    }
    std::size_t blockIndex{get_blocks_index(index)};
    std::size_t eleIndex{index - blockIndex * _chunk_size};
    return _map[blockBeginIndex + blockIndex][eleIndex];
  };
  reference operator[](std::size_t index) const {
    std::size_t headSize{_map[_head._blockIndex].size()};
    std::size_t blockBeginIndex{_head._blockIndex};
    if (index >= headSize) {
      index -= headSize;
      ++blockBeginIndex;
    }
    std::size_t blockIndex{get_blocks_index(index)};
    std::size_t eleIndex{index - blockIndex * _chunk_size};
    return _map[blockBeginIndex + blockIndex][eleIndex];
  };

  reference at(std::size_t index) {
    validateIndex(index);
    return (*this)[index];
  };
  reference at(std::size_t index) const {
    validateIndex(index);
    return (*this)[index];
  };

  reference front() { return _map[_head._blockIndex][_head._currentIndex]; };
  reference front() const {
    return _map[_head._blockIndex][_head._currentIndex];
  };

  reference back() { return _map[_tail._blockIndex][_tail._currentIndex - 1]; };
  reference back() const {
    return _map[_tail._blockIndex][_tail._currentIndex - 1];
    ;
  };

  /* modifiers */
  void push_back(const_reference value) {
    if (!_map[_tail._blockIndex].is_full()) {
      _map[_tail._blockIndex].push_back(value);
      ++_tail._currentIndex;
    } else if (++_tail._blockIndex != _map.size()) {
      if (!_map[_tail._blockIndex].capacity()) {
        _map[_tail._blockIndex] = pop_or_make_spare_left_block();
      }
      _map[_tail._blockIndex].push_back(value);
      _tail._currentIndex = _map[_tail._blockIndex].size();
    } else {
      _map.push_back(pop_or_make_spare_left_block());
      _map[_tail._blockIndex].push_back(value);
      _tail._currentIndex = _map[_tail._blockIndex].size();
    }
    ++_size;
  };

  void push_back(rvalue_reference value) {
    if (!_map[_tail._blockIndex].is_full()) {
      _map[_tail._blockIndex].push_back(std::move(value));
      ++_tail._currentIndex;
    } else if (++_tail._blockIndex != _map.size()) {
      if (!_map[_tail._blockIndex].capacity()) {
        _map[_tail._blockIndex] = pop_or_make_spare_left_block();
      }
      _map[_tail._blockIndex].push_back(std::move(value));
      _tail._currentIndex = _map[_tail._blockIndex].size();
    } else {
      _map.push_back(pop_or_make_spare_left_block());
      _map[_tail._blockIndex].push_back(std::move(value));
      _tail._currentIndex = _map[_tail._blockIndex].size();
    }
    ++_size;
  };

  void push_front(const_reference value) {
    if (!_map[_head._blockIndex].is_full()) {
      _map[_head._blockIndex].push_front(value);
      if (_head._blockIndex == _tail._blockIndex) {
        ++_tail._currentIndex;
      }
    } else if (_head._blockIndex != 0) {
      --_head._blockIndex;
      if (!_map[_head._blockIndex].capacity()) {
        _map[_head._blockIndex] = pop_or_make_spare_right_block();
      }
      _map[_head._blockIndex].push_front(value);
    } else {
      _map.push_front(pop_or_make_spare_right_block());
      _map[_head._blockIndex].push_front(value);
      ++_tail._blockIndex;
      _tail._currentIndex = _map[_tail._blockIndex].size();
    }
    ++_size;
  };

  void push_front(rvalue_reference value) {
    if (!_map[_head._blockIndex].is_full()) {
      _map[_head._blockIndex].push_front(std::move(value));
      if (_head._blockIndex == _tail._blockIndex) {
        ++_tail._currentIndex;
      }
    } else if (_head._blockIndex != 0) {
      --_head._blockIndex;
      if (!_map[_head._blockIndex].capacity()) {
        _map[_head._blockIndex] = pop_or_make_spare_right_block();
      }
      _map[_head._blockIndex].push_front(std::move(value));
    } else {
      _map.push_front(pop_or_make_spare_right_block());
      _map[_head._blockIndex].push_front(std::move(value));
      ++_tail._blockIndex;
      _tail._currentIndex = _map[_tail._blockIndex].size();
    }
    ++_size;
  };

  value pop_front() {
    buffer& headBlock{_map[_head._blockIndex]};
    value ele{headBlock.pop_front()};
    if (headBlock.empty()) {
      ++_head._blockIndex;
    } else if (_head._blockIndex == _tail._blockIndex) {
      --_tail._currentIndex;
    }
    --_size;
    return ele;
  };
  value pop_back() {
    buffer& tailBlock{_map[_tail._blockIndex]};
    value ele{tailBlock.pop_back()};
    if (tailBlock.empty()) {
      --_tail._blockIndex;
      _tail._currentIndex = _chunk_size;
    } else {
      --_tail._currentIndex;
    }
    --_size;
    return ele;
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

  std::size_t get_blocks_index(std::size_t size) const {
    return size * (1.0 / _chunk_size);
  };

  void validateIndex(std::size_t index) {
    if (index >= size()) {
      throw OutOfRangeException{"Index out of range"};
    }
  }

  buffer pop_or_make_spare_left_block() {
    if (_head._blockIndex != 0 && _map.front().capacity()) {
      --_tail._blockIndex;
      return _map.pop_front();
    }
    return buffer();
  }

  buffer pop_or_make_spare_right_block() {
    return _tail._blockIndex != _map.size() - 1 && _map.back().capacity()
               ? _map.pop_back()
               : buffer();
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
    std::size_t _blockIndex{};
    std::size_t _currentIndex{};
    const Deque<T>* _deque_ptr{};

    Iterator(std::size_t blockIndex, std::size_t currentIndex,
             Deque<T>* deque_ptr)
        : _blockIndex{blockIndex}, _currentIndex{currentIndex},
          _deque_ptr{deque_ptr} {};

  public:
    reference operator*() {
      return _deque_ptr->_map[_blockIndex][_currentIndex];
    };
    pointer operator->() { return &(**this); };

    Iterator& operator++() {
      ++_currentIndex;
      if (_blockIndex != _deque_ptr->_tail._blockIndex &&
          _currentIndex == _deque_ptr->_map[_blockIndex].size()) {
        // jump to the next block
        ++_blockIndex;
        _currentIndex = 0;
      }
      return *this;
    }

    Iterator operator++(int) const {
      Iterator tmp{*this};
      ++(*this);
      return tmp;
    }

    Iterator& operator+=(difference_type distance) {
      for (int i{0}; i < distance; ++i) {
        ++(*this);
      }
      // typename buffer::iterator::difference_type offset(_current + index);
      // if (offset >= _chunk_size) {
      //   typename map::iterator::difference_type blockOffset(offset /
      //                                                       _chunk_size);
      //   typename buffer::iterator::difference_type currentOffset(
      //       offset - blockOffset * _chunk_size);
      //   _block += blockOffset;
      //   _current = _block->begin() + currentOffset;
      // } else {
      //   _current = offset;
      // }
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
      if (_currentIndex == 0) {
        --_blockIndex;
        _currentIndex = _deque_ptr->_map[_blockIndex].size() - 1;
      } else {
        --_currentIndex;
      }
      return *this;
    }

    Iterator operator--(int) const {
      Iterator tmp{*this};
      --(*this);
      return tmp;
    }

    Iterator& operator-=(difference_type distance) {
      for (int i{0}; i < distance; ++i) {
        --(*this);
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
      return _blockIndex * _chunk_size + _currentIndex -
             (other._blockIndex * other._chunk_size + other._currentIndex);
    }

    bool operator==(const Iterator& other) const {
      return _blockIndex == other._blockIndex &&
             _currentIndex == other._currentIndex;
    }

    bool operator!=(const Iterator& other) const { return !(*this == other); }

    friend bool operator<(const Iterator& i1, const Iterator& i2) {
      return i1._blockIndex == i2._blockIndex
                 ? i1._currentIndex < i2._currentIndex
                 : i1._blockIndex < i2._blockIndex;
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
      return *this;
    }
  };
};