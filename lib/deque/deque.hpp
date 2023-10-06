#pragma once

#include <allocator.hpp>
#include <array.hpp>
#include <concept.hpp>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector.hpp>

inline constexpr std::size_t DEFAULT_DEQUE_BUFFER_SIZE{4096};
inline constexpr std::size_t DEFAULT_DEQUE_MAP_MIN_SIZE{8};

template <typename T> class Deque {
  class Iterator;
  class ReverseIterator;

  using iterator = Iterator;
  using reverse_iterator = ReverseIterator;
  using pointer = T*;
  using reference = T&;
  using rvalue_reference = T&&;
  using const_reference = const T&;
  using value = T;
  using self = Deque<T>;

  class OutOfRangeException : public std::exception {
  private:
    std::string message;

  public:
    OutOfRangeException(std::string msg) : message{msg} {}
    std::string what() { return message; }
  };

private:
  inline constexpr static std::size_t _chunk_size{
      sizeof(T) > DEFAULT_DEQUE_BUFFER_SIZE
          ? 16
          : DEFAULT_DEQUE_BUFFER_SIZE / sizeof(T)};

  std::size_t get_nodes_count(std::size_t size) const {
    return size / _chunk_size + 1;
  }

  using buffer = Array<T, _chunk_size>;
  using map = Vector<buffer>;

  map _map{};
  std::size_t _size{};
  iterator _head{};
  iterator _tail{};

public:
  /* Constructors */
  Deque() = delete;
  Deque(std::size_t count, const_reference defaultValue = value());
  template <typename InputIter> Deque(InputIter begin, InputIter end);
  Deque(std::initializer_list<value> list);

  ~Deque();

  Deque(const self& copy);
  self& operator=(const self& copy);

  Deque(self&& move) noexcept;
  self& operator=(self&& move) noexcept;

  /* iterator */
  iterator begin();
  iterator end();

  iterator begin() const;
  iterator end() const;

  iterator cbegin() const;
  iterator cend() const;

  reverse_iterator rbegin();
  reverse_iterator rend();

  reverse_iterator rbegin() const;
  reverse_iterator rend() const;

  reverse_iterator crbegin() const;
  reverse_iterator crend() const;

  /* Capacity */
  bool empty() const { return _size == 0; } noexcept;
  std::size_t size() const { return _size; } noexcept;
  void shrink_to_fit();

  /* Element access */
  reference operator[](int index) { return _elements[index]; };
  const_reference operator[](int index) const { return _elements[index]; };

  reference at(int index) {
    validateIndex(index);
    return _elements[index];
  };

  const_reference at(int index) const {
    validateIndex(index);
    return _elements[index];
  };

  reference front() { return _elements[0]; };
  const_reference front() const { return _elements[0]; };

  reference back() { return _elements[_size - 1]; };
  const_reference back() const { return _elements[_size - 1]; };

  pointer data() { return _elements; };
  const pointer data() const { return _elements; };

  /* modifiers */
  void push_back(reference value);
  void push_back(rvalue_reference value);

  void push_front(reference value);
  void push_front(rvalue_reference value);

  void pop_back();
  void pop_front();

  iterator insert(iterator pos, reference value);
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

  friend void swap(self& a, self& b) noexcept { a.swap(b) }

  /* helpers */
private:
  void validateIndex(std::size_t index) const {
    if (index < 0 || index > _size) {
      throw OutOfRangeException{"Index out of range"};
    }
  };
};

#include <deque.cpp>