#pragma once

#include <allocator.hpp>
#include <concept.hpp>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>

template <typename T, std::size_t N, typename Allocator = Allocator<T>>
class Array {
  class OutOfRangeException : public std::exception {
  private:
    std::string message;

  public:
    OutOfRangeException(std::string msg) : message{msg} {}
    std::string what() { return message; }
  };

public:
  using self = Array<T, N, Allocator>;
  using allocator_type = Allocator;

private:
  allocator_type _allocator{};
  std::size_t _size{};
  T* _elements{};

  void validateIndex(int index) const {
    if (index < 0 || index > _size) {
      throw OutOfRangeException{"Index out of range"};
    }
  };

public:
  /* Constructors */
  Array();
  Array(const T& defaultValue);
  Array(std::initializer_list<T> list);
  ~Array();

  Array(const self& copy);
  self& operator=(const self& copy);

  Array(self&& move) noexcept;
  self& operator=(self&& move) noexcept;

  /* Iterators */
  class Iterator;
  class ReverseIterator;

  using iterator = Iterator;
  using reverse_iterator = ReverseIterator;

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
  bool empty() const { return _size == 0; };
  std::size_t size() const { return _size; };

  /* Element access */
  T& operator[](int index) { return _elements[index]; };
  const T& operator[](int index) const { return _elements[index]; };

  T& at(int index) {
    validateIndex(index);
    return _elements[index];
  };

  const T& at(int index) const {
    validateIndex(index);
    return _elements[index];
  };

  T& front() { return _elements[0]; };
  const T& front() const { return _elements[0]; };

  T& back() { return _elements[_size - 1]; };
  const T& back() const { return _elements[_size - 1]; };

  T* data() { return _elements; };
  const T* data() const { return _elements; };

  void swap(self& other) noexcept {
    using std::swap;
    swap(_size, other._size);
    swap(_elements, other._elements);
  }

  friend void swap(self& a, self& b) noexcept { a.swap(b); };
};

#include <array.cpp>