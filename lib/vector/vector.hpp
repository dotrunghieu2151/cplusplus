#pragma once

#include <concept.hpp>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>

template <typename T> class Vector {
  inline static constexpr double growRate{1.5};

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
  void validateIndex(int index) const {
    if (index < 0 || index > _size) {
      throw OutOfRangeException{"Index out of range"};
    }
  };

public:
  /* Constructors */
  Vector(std::size_t capacity);
  Vector(std::initializer_list<T> list);
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
  void pop_back();

  template <typename... Args> void emplace_back(Args&&... args);
  void insert(int insertIndex, const T& ele);
  void insert(int insertIndex, T&& ele);
  template <typename... Args> void insert(int insertIndex, Args&&... args);
  template <concepts::IsIterator Input>
  void insert(int insertIndex, Input first, Input last);
  void erase(int index);
  void erase(int startIndex, int endIndex);

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

  void swap(Vector<T>& other) noexcept {
    using std::swap;
    swap(_size, other._size);
    swap(_space, other._space);
    swap(_elements, other._elements);
  }
  friend void swap(Vector<T>& a, Vector<T>& b) noexcept { a.swap(b); };
};

#include <vector.cpp>