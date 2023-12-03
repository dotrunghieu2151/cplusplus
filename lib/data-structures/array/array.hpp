#pragma once

#include <concept.hpp>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>

template <typename T, std::size_t N> class Array {
  class OutOfRangeException : public std::exception {
  private:
    std::string message;

  public:
    OutOfRangeException(std::string msg) : message{msg} {}
    std::string what() { return message; }
  };

public:
  using self = Array<T, N>;

private:
  T _elements[N]{};

  void validateIndex(int index) const {
    if (index < 0 || index > N) {
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
  template <bool IsConst> class Iterator;
  class ReverseIterator;

  using iterator = Iterator<false>;
  using const_iterator = Iterator<true>;
  using reverse_iterator = ReverseIterator;

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

  const_iterator cbegin() const;
  const_iterator cend() const;

  reverse_iterator rbegin();
  reverse_iterator rend();

  reverse_iterator rbegin() const;
  reverse_iterator rend() const;

  reverse_iterator crbegin() const;
  reverse_iterator crend() const;

  /* Capacity */
  bool empty() const { return N == 0; };
  std::size_t size() const { return N; };

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

  T& back() { return _elements[N - 1]; };
  const T& back() const { return _elements[N - 1]; };

  T* data() { return _elements; };
  const T* data() const { return _elements; };

  void swap(self& other) noexcept {
    using std::swap;
    swap(_elements, other._elements);
  }

  friend void swap(self& a, self& b) noexcept { a.swap(b); };
};

template <class... T>
Array(T&&... t) -> Array<std::common_type_t<T...>, sizeof...(T)>;

#include <array.cpp>