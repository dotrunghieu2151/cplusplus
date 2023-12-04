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

#define QUEUE_DEBUG 0

#if QUEUE_DEBUG == 1
#define QUEUE_DEBUG_MS(mes)                                                    \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define QUEUE_DEBUG_MS(mes)                                                    \
  do {                                                                         \
  } while (0)
#endif

template <typename T, typename Allocator = Allocator<T>> class Queue {
  inline static constexpr double growRate{2};

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
  using self = Queue<T>;

  Queue() = default;

  Queue(std::size_t capacity)
      : _capacity{capacity}, _elements{_allocator.allocate(capacity)},
        _head{_elements}, _tail{_head} {
    QUEUE_DEBUG_MS("Queue Ctor capacity");
  };
  Queue(std::initializer_list<T> list) : Queue(list.size() + 2) {
    // helpers::printf("Queue list ctor");
    for (const T& i : list) {
      _allocator.construct(_tail, i);
      ++_tail;
    }
  };

  ~Queue() {
    QUEUE_DEBUG_MS("Queue Dtor");
    for (pointer i{_head}; i != _tail; ++i) {
      _allocator.destruct(i);
    };
    _allocator.deallocate(_elements);
  };

  Queue(const self& other)
      : _allocator{other._allocator}, _capacity{other._capacity},
        _elements{_allocator.allocate(_capacity)},
        _head{_elements + (other._head - other._elements)},
        _tail{_elements + (other._tail - other._elements)} {
    QUEUE_DEBUG_MS("Queue Copy Ctor");
    for (pointer i{other._head}, ele{_head}; i < other._tail; ++i, ++ele) {
      _allocator.construct(ele, *i);
    };
  };

  Queue(self&& other) noexcept
      : _allocator{std::move(other._allocator)}, _capacity{other._capacity},
        _elements{other._elements}, _head{other._head}, _tail{other._tail} {
    QUEUE_DEBUG_MS("Queue Move Ctor");
    other._capacity = 0;
    other._head = nullptr;
    other._tail = nullptr;
    other._elements = nullptr;
  };

  self& operator=(const self& other) {
    QUEUE_DEBUG_MS("Queue Copy Operator");
    self copy{other};
    copy.swap(*this);
    return *this;
  };

  self& operator=(self&& other) noexcept {
    QUEUE_DEBUG_MS("Queue Move Operator");
    self move{std::move(other)};
    move.swap(*this);
    return *this;
  };

  void shrink_to_fit(){};
  void resize(){};

  void push_back(const_reference element) {
    if (_tail == _elements + _capacity && !is_full()) {
      shiftLeft(end(), 1);
      --_tail;
    } else if (is_full()) {
      std::size_t newCapacity{getNewCapacity()};
      reallocate(newCapacity);
      return push_back(element);
    }
    _allocator.construct(_tail, element);
    ++_tail;
  };
  void push_back(rvalue_reference element) {
    if (_tail == _elements + _capacity && !is_full()) {
      shiftLeft(end(), 1);
      --_tail;
    } else if (is_full()) {
      std::size_t newCapacity{getNewCapacity()};
      reallocate(newCapacity);
      return push_back(std::move(element));
    }
    _allocator.construct(_tail, std::move(element));
    ++_tail;
  };

  value pop_front() {
    value ele{std::move(front())};
    ++_head;
    return ele;
  };

  iterator insert(iterator pos, const_reference element) {
    if (!is_full()) {
      if (_tail + 1 <= _elements + _capacity) {
        shiftRight(pos._current - 1, 1);
        _allocator.construct(pos._current, element);
        return {pos._current};
      } else {
        shiftLeft(pos._current, 1);
        _allocator.construct(pos._current - 1, element);
        return {pos._current - 1};
      }
    } else {
      difference_type distance{pos._current - _head};
      std::size_t newCapacity{getNewCapacity()};
      reallocate(newCapacity);
      return insert(iterator{_head + distance}, element);
    }
  };

  iterator insert(iterator pos, rvalue_reference element) {
    if (!is_full()) {
      if (_tail + 1 <= _elements + _capacity) {
        shiftRight(pos._current - 1, 1);
        _allocator.construct(pos._current, std::move(element));
        return {pos._current};
      } else {
        shiftLeft(pos._current, 1);
        _allocator.construct(pos._current - 1, std::move(element));
        return {pos._current - 1};
      }
    } else {
      difference_type distance{pos._current - _head};
      std::size_t newCapacity{getNewCapacity()};
      reallocate(newCapacity);
      return insert(iterator{_head + distance}, std::move(element));
    }
  };

  template <typename InputIterator>
  iterator insert(iterator pos, InputIterator start, InputIterator end) {
    std::size_t totalNewBlocks{static_cast<std::size_t>(end - start)};
    difference_type distance{pos._current - _head};
    if (size() + totalNewBlocks <= _capacity) {
      std::size_t freeBlocksAfterInsert{_capacity - (size() + totalNewBlocks)};
      std::size_t freeBlocksEachSideAfterInsert{
          static_cast<std::size_t>(std::round(freeBlocksAfterInsert * 0.5))};
      std::size_t leftFreeBlocks{static_cast<std::size_t>(_head - _elements)};
      std::size_t rightFreeBlocks(_capacity + _elements - _tail);
      std::size_t leftMoveDistance{
          leftFreeBlocks > freeBlocksEachSideAfterInsert
              ? leftFreeBlocks - freeBlocksEachSideAfterInsert
              : 0};
      if (leftMoveDistance) {
        shiftLeft(pos._current, leftMoveDistance);
      }
      std::size_t remainingFreeBlocks{
          freeBlocksEachSideAfterInsert > leftMoveDistance
              ? freeBlocksEachSideAfterInsert - leftMoveDistance
              : 0};
      std::size_t rightMoveDistance{
          rightFreeBlocks > freeBlocksEachSideAfterInsert + remainingFreeBlocks
              ? rightFreeBlocks - freeBlocksEachSideAfterInsert -
                    remainingFreeBlocks
              : 0};
      if (rightMoveDistance) {
        shiftRight(pos._current - 1, rightMoveDistance);
      }
      std::size_t i{};
      while (start != end) {
        _allocator.construct(_head + distance + i, *start);
        ++start;
        ++i;
      }
      return {_head + distance};
    } else {
      std::size_t newCapacity{getNewCapacity(size() + totalNewBlocks)};
      reallocate(newCapacity);
      return insert(iterator{_head + distance}, start, end);
    }
  }

  iterator erase(iterator start, iterator end) {
    difference_type distance{end - start};
    for (iterator i{start}; i != end; ++i) {
      _allocator.destruct(i._current);
    }
    for (pointer i{start._current}; i < _tail - distance; ++i) {
      _allocator.construct(i, std::move(*(i + distance)));
    }
    _tail -= distance;
    return {start._current};
  };

  iterator erase(iterator pos) {
    _allocator.destruct(pos._current);
    for (pointer i{pos._current}; i < _tail - 1; ++i) {
      _allocator.construct(i, std::move(*(i + 1)));
    }
    --_tail;
    return {pos._current};
  };

  reference front() { return *_head; };
  reference front() const { return *_head; };

  reference back() { return *(_tail - 1); };
  reference back() const { return *(_tail - 1); };

  pointer data() { return _elements; };
  pointer data() const { return _elements; };

  reference at(std::size_t index) {
    validateIndex(index);
    return (*this)[index];
  };
  reference at(std::size_t index) const {
    validateIndex(index);
    return (*this)[index];
  };

  reference operator[](std::size_t index) { return *(_head + index); };
  reference operator[](std::size_t index) const { return *(_head + index); };

  bool empty() const noexcept { return size() == 0; };
  bool is_full() const noexcept { return size() == _capacity; };
  std::size_t capacity() const noexcept { return _capacity; };
  std::size_t size() const noexcept {
    return static_cast<std::size_t>(_tail - _head);
  };

  iterator begin() { return iterator{_head}; };
  iterator end() { return iterator{_tail}; };
  iterator begin() const { return iterator{_head}; };
  iterator end() const { return iterator{_tail}; };
  iterator cbegin() const { return iterator{_head}; };
  iterator cend() const { return iterator{_tail}; };

  void swap(self& other) noexcept {
    using std::swap;
    swap(_allocator, other._allocator);
    swap(_capacity, other._capacity);
    swap(_elements, other._elements);
    swap(_head, other._head);
    swap(_tail, other._tail);
  }

  void friend swap(self& e1, self& e2) noexcept { e1.swap(e2); };

private:
  Allocator _allocator{};
  std::size_t _capacity{};
  pointer _elements{};
  pointer _head{};
  pointer _tail{};

  void validateIndex(std::size_t index) const {
    if (index >= size()) {
      throw OutOfRangeException{"Index out of range"};
    }
  };

  std::size_t getNewCapacity(std::size_t minCapacity = 2) const {
    return std::max(minCapacity,
                    static_cast<std::size_t>(std::round(growRate * _capacity)));
  }

  void reallocate(std::size_t capacity) {
    // helpers::printf("Reallocating with ", capacity);
    pointer newSpace{_allocator.allocate(capacity)};

    pointer newHead{newSpace};
    pointer newTail{newHead};
    for (pointer i{_head}; i < _tail; ++i) {
      _allocator.construct(newTail, std::move(*i));
      _allocator.destruct(i);
      ++newTail;
    }
    std::swap(_elements, newSpace);
    _allocator.deallocate(newSpace);
    _capacity = capacity;
    _head = newHead;
    _tail = newTail;
  }

  void shiftLeft(iterator startPos, std::size_t units = 1) {
    if (_head - units < _elements) {
      throw std::runtime_error("shiftLeft overflow");
    }
    for (pointer i{_head}; i < startPos._current; ++i) {
      pointer pos{i - units};
      _allocator.construct(pos, std::move(*i));
    }
    _head -= units;
  }

  void shiftRight(iterator startPos, std::size_t units = 1) {
    if (_tail + units > _elements + _capacity) {
      throw std::runtime_error("shiftRight overflow");
    }
    for (pointer i{_tail - 1}; i > startPos._current; --i) {
      pointer pos{i + units};
      _allocator.construct(pos, std::move(*i));
    }
    _tail += units;
  }

public:
  class OutOfRangeException : public std::exception {
    friend class Queue;

  private:
    std::string message;
    OutOfRangeException(std::string msg) : message{msg} {}
    std::string what() { return message; }
  };

  class Iterator {
    friend class Queue;

  public:
    using iterator_category = Queue::iterator_category;
    using difference_type = Queue::difference_type;

    Iterator() = default;

  private:
    Iterator(pointer current) : _current{current} {};
    pointer _current;

  public:
    reference operator*() { return *_current; };
    pointer operator->() { return _current; };

    Iterator& operator++() {
      ++_current;
      return *this;
    }

    Iterator operator++(int) {
      Iterator tmp{*this};
      ++_current;
      return tmp;
    }

    Iterator& operator+=(int index) {
      _current += index;
      return *this;
    }

    Iterator operator+(const difference_type index) const {
      return _current + index;
    }

    friend Iterator operator+(const difference_type index,
                              const Iterator& other) {
      return other + index;
    }

    Iterator& operator--() {
      --_current;
      return *this;
    }

    Iterator operator--(int) {
      Iterator tmp{*this};
      --_current;
      return *this;
    }

    Iterator& operator-=(int index) {
      _current -= index;
      return *this;
    }

    Iterator operator-(const difference_type index) const {
      return _current - index;
    }

    friend Iterator operator-(const difference_type index,
                              const Iterator& other) {
      return other - index;
    }

    difference_type operator-(const Iterator& other) const {
      return _current > other._current ? _current - other._current
                                       : other._current - _current;
    }

    bool operator==(const Iterator& other) const {
      return _current == other._current;
    }

    bool operator!=(const Iterator& other) const { return !(*this == other); }

    reference operator[](difference_type index) const {
      return _current[index];
    }
  };
};