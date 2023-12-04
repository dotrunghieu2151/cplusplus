#pragma once

#include <concept.hpp>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <utility>

template <typename T> class Vector {
  inline static constexpr double growRate{1.5};

public:
  using value_type = T;
  using reference = T&;
  using rvalue_reference = T&&;

  class OutOfRangeException : public std::exception {
  private:
    std::string message;

  public:
    OutOfRangeException(std::string msg) : message{msg} {}
    std::string what() { return message; }
  };

private:
  std::size_t _size{};
  std::size_t _space{};
  T* _elements{};

  void reallocateIfRequired();
  void reallocate(std::size_t capacity);
  void validateIndex(std::size_t index) const {
    if (index >= _size) {
      throw OutOfRangeException{"Index out of range"};
    }
  };

public:
  /* Constructors */
  Vector(std::size_t capacity);
  Vector(std::initializer_list<T> list);

  template <std::random_access_iterator Iter> Vector(Iter begin, Iter end);
  ~Vector();

  Vector(const Vector<T>& copy);
  Vector<T>& operator=(const Vector<T>& copy);

  Vector(Vector<T>&& move) noexcept;
  Vector<T>& operator=(Vector<T>&& move) noexcept;

  /* Iterators */
  class Iterator;
  Iterator begin();
  Iterator end();

  const Iterator cbegin() const;
  const Iterator cend() const;

  /* Capacity */
  bool empty() const { return _size == 0; };
  std::size_t capacity() const { return _space; };
  std::size_t size() const { return _size; };

  // Requests a change in capacity
  // reserve() will never decrase the capacity.
  void reserve(std::size_t newLength);

  // Changes the Vector's size.
  // If the newsize is smaller, the last elements will be lost.
  // Has a default value param for custom values when resizing.
  void resize(std::size_t newSize, const T& = T());

  void shrink_to_fit();

  /* Modifiers */
  // Removes all elements from the Vector
  // Capacity is not changed.
  void clear();
  void push_back(T&& val);
  void push_back(const T& val);
  value_type pop_back();

  template <typename... Args> void emplace_back(Args&&... args);
  void insert(std::size_t insertIndex, const T& ele);
  void insert(std::size_t insertIndex, T&& ele);
  template <typename... Args>
  void insert(std::size_t insertIndex, Args&&... args);
  template <concepts::IsIterator Input>
  void insert(std::size_t insertIndex, Input first, Input last);
  void erase(std::size_t index);
  void erase(std::size_t startIndex, std::size_t endIndex);

  /* Element access */
  T& operator[](std::size_t index) { return _elements[index]; };
  const T& operator[](std::size_t index) const { return _elements[index]; };

  T& at(std::size_t index) {
    validateIndex(index);
    return _elements[index];
  };

  const T& at(std::size_t index) const {
    validateIndex(index);
    return _elements[index];
  };

  T& front() { return _elements[0]; };
  const T& front() const { return _elements[0]; };

  T& back() { return _elements[_size - 1]; };
  const T& back() const { return _elements[_size - 1]; };

  T* data() { return _elements; };
  const T* data() const { return _elements; };

  void swap(Vector<T>& other) noexcept {
    using std::swap;
    swap(_size, other._size);
    swap(_space, other._space);
    swap(_elements, other._elements);
  }
  friend void swap(Vector<T>& a, Vector<T>& b) noexcept { a.swap(b); }

  friend std::ostream& operator<<(std::ostream& stream, const Vector& vector) {
    auto el{vector.cbegin()};
    stream << "[";
    if (el != vector.cend()) {
      stream << *el;
      for (++el; el != vector.cend(); ++el)
        stream << ", " << *el;
    }
    stream << "]";
    return stream;
  }
};

#include <vector.cpp>