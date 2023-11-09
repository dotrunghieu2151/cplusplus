#include <algorithm>
#include <allocator.hpp>
#include <array.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <helpers.hpp>
#include <initializer_list>
#include <iostream>
#include <iterator> // For std::forward_iterator_tag
#include <new>      // for (T*)::operator new (sizeof(T)) ::operator delete(p)
#include <type_traits>
#include <utility>

/* Constructors */
using std::cout;
using std::endl;

template <typename T, std::size_t N, typename Allocator>
Array<T, N, Allocator>::Array()
    : _size{N}, _elements{_allocator.allocate(_size)} {
  helpers::printf("Array Ctor default");
}

template <typename T, std::size_t N, typename Allocator>
Array<T, N, Allocator>::Array(const T& defaultValue) : Array() {
  for (std::size_t i{0}; i < _size; ++i) {
    _allocator.construct(_elements + i, defaultValue);
  }
  helpers::printf("Array Ctor default value");
}

template <typename T, std::size_t N, typename Allocator>
Array<T, N, Allocator>::Array(std::initializer_list<T> list) : Array() {
  helpers::printf("Array Ctor List");

  for (std::size_t index{0}; index < list.size(); ++index) {
    _allocator.construct(_elements + index, *(list.begin() + index));
  }
}

template <typename T, std::size_t N, typename Allocator>
Array<T, N, Allocator>::~Array() {
  helpers::printf("Array Dtor");
  for (std::size_t i{0}; i < _size; ++i) {
    _allocator.destruct(_elements + i);
  }
  _allocator.deallocate(_elements);
}

template <typename T, std::size_t N, typename Allocator>
Array<T, N, Allocator>::Array(const Array<T, N, Allocator>& copy)
    : _size{copy._size}, _elements{_allocator.allocate(_size)} {
  helpers::printf("Array Copy Ctor");

  for (std::size_t index{0}; index < _size; ++index) {
    _allocator.construct(_elements + index, copy._elements[index]);
  }
}

template <typename T, std::size_t N, typename Allocator>
Array<T, N, Allocator>&
Array<T, N, Allocator>::operator=(const Array<T, N, Allocator>& copy) {
  helpers::printf("Array copy");

  Array<T, N, Allocator> temp{copy};
  temp.swap(*this);
  return *this;
}

template <typename T, std::size_t N, typename Allocator>
Array<T, N, Allocator>::Array(Array<T, N, Allocator>&& move) noexcept
    : _size{move._size}, _elements{move._elements} {
  helpers::printf("Array move Ctor");

  move._size = 0;
  move._elements = nullptr;
}

template <typename T, std::size_t N, typename Allocator>
Array<T, N, Allocator>&
Array<T, N, Allocator>::operator=(Array<T, N, Allocator>&& move) noexcept {
  helpers::printf("Array Move assignment");

  Array<T, N, Allocator> tempMove{std::move(move)};
  tempMove.swap(*this);
  return *this;
}

/* Iterator */
template <typename T, std::size_t N, typename Allocator>
class Array<T, N, Allocator>::Iterator {
public:
  using iterator_category = std::contiguous_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using reference = T&;

private:
  T* _current;

public:
  Iterator(T* p) : _current{p} {}

  reference operator*() const { return *_current; }
  pointer operator->() { return _current; }

  Iterator& operator++() {
    ++_current;
    return *this;
  }

  // post-fix increment
  Iterator operator++(int) {
    Iterator tmp = *this;
    ++_current;
    return tmp;
  }

  Iterator operator+(const difference_type index) const {
    return _current + index;
  }
  friend Iterator operator+(const difference_type index,
                            const Iterator& other) {
    return other + index;
  }

  Iterator& operator+=(int index) {
    _current += index;
    return *this;
  }

  Iterator& operator--() {
    --_current;
    return *this;
  }

  // post-fix increment
  Iterator operator--(int) {
    Iterator tmp = *this;
    --_current;
    return tmp;
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
    return _current - other._current;
  }

  bool operator==(const Iterator& other) const {
    return _current == other._current;
  }

  bool operator!=(const Iterator& other) const {
    return _current != other._current;
  }

  // for random access
  reference operator[](difference_type index) const { return _current[index]; }
};

template <typename T, std::size_t N, typename Allocator>
class Array<T, N, Allocator>::ReverseIterator {
public:
  using iterator_category = std::contiguous_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using reference = T&;

private:
  T* _current;

public:
  ReverseIterator(T* p) : _current{p} {}

  reference operator*() const { return *(_current - 1); }
  pointer operator->() { return _current - 1; }

  ReverseIterator& operator++() {
    --_current;
    return *this;
  }

  // post-fix increment
  ReverseIterator operator++(int) {
    ReverseIterator tmp = *this;
    --_current;
    return tmp;
  }

  ReverseIterator operator+(const difference_type index) const {
    return _current - index;
  }
  friend ReverseIterator operator+(const difference_type index,
                                   const ReverseIterator& other) {
    return other - index;
  }

  ReverseIterator& operator+=(int index) {
    _current -= index;
    return *this;
  }

  ReverseIterator& operator--() {
    ++_current;
    return *this;
  }

  // post-fix increment
  ReverseIterator operator--(int) {
    ReverseIterator tmp = *this;
    ++_current;
    return tmp;
  }

  ReverseIterator& operator-=(int index) {
    _current += index;
    return *this;
  }

  ReverseIterator operator-(const difference_type index) const {
    return _current + index;
  }
  friend ReverseIterator operator-(const difference_type index,
                                   const ReverseIterator& other) {
    return other + index;
  }

  difference_type operator-(const ReverseIterator& other) const {
    return _current + other._current;
  }

  bool operator==(const ReverseIterator& other) const {
    return _current == other._current;
  }

  bool operator!=(const ReverseIterator& other) const {
    return _current != other._current;
  }

  // for random access
  reference operator[](difference_type index) const { return _current[index]; }
};

template <typename T, std::size_t N, typename Allocator>
typename Array<T, N, Allocator>::iterator Array<T, N, Allocator>::begin() {
  return iterator{&_elements[0]};
}

template <typename T, std::size_t N, typename Allocator>
typename Array<T, N, Allocator>::iterator
Array<T, N, Allocator>::begin() const {
  return iterator{&_elements[0]};
}

template <typename T, std::size_t N, typename Allocator>
typename Array<T, N, Allocator>::iterator
Array<T, N, Allocator>::cbegin() const {
  return iterator{&_elements[0]};
}

template <typename T, std::size_t N, typename Allocator>
typename Array<T, N, Allocator>::iterator Array<T, N, Allocator>::end() {
  return iterator{&_elements[_size]};
}

template <typename T, std::size_t N, typename Allocator>
typename Array<T, N, Allocator>::iterator Array<T, N, Allocator>::end() const {
  return iterator{&_elements[_size]};
}

template <typename T, std::size_t N, typename Allocator>
typename Array<T, N, Allocator>::iterator Array<T, N, Allocator>::cend() const {
  return iterator{&_elements[_size]};
}

template <typename T, std::size_t N, typename Allocator>
typename Array<T, N, Allocator>::reverse_iterator
Array<T, N, Allocator>::rend() {
  return reverse_iterator{&_elements[0]};
}

template <typename T, std::size_t N, typename Allocator>
typename Array<T, N, Allocator>::reverse_iterator
Array<T, N, Allocator>::rend() const {
  return reverse_iterator{&_elements[0]};
}

template <typename T, std::size_t N, typename Allocator>
typename Array<T, N, Allocator>::reverse_iterator
Array<T, N, Allocator>::crend() const {
  return reverse_iterator{&_elements[0]};
}

template <typename T, std::size_t N, typename Allocator>
typename Array<T, N, Allocator>::reverse_iterator
Array<T, N, Allocator>::rbegin() {
  return reverse_iterator{&_elements[_size]};
}

template <typename T, std::size_t N, typename Allocator>
typename Array<T, N, Allocator>::reverse_iterator
Array<T, N, Allocator>::rbegin() const {
  return reverse_iterator{&_elements[_size]};
}

template <typename T, std::size_t N, typename Allocator>
typename Array<T, N, Allocator>::reverse_iterator
Array<T, N, Allocator>::crbegin() const {
  return reverse_iterator{&_elements[_size]};
}