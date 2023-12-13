#include <algorithm>
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

template <typename T, std::size_t N> Array<T, N>::Array() : _elements{} {
  helpers::printf("Array Ctor default");
}

template <typename T, std::size_t N>
Array<T, N>::Array(const T& defaultValue) : Array() {
  for (std::size_t i{}; i < N; ++i) {
    _elements[i] = defaultValue;
  }
  helpers::printf("Array Ctor default value");
}

template <typename T, std::size_t N>
Array<T, N>::Array(std::initializer_list<T> list) : Array() {
  helpers::printf("Array Ctor List");

  for (std::size_t index{0}; index < list.size(); ++index) {
    _elements[index] = *(list.begin() + index);
  }
}

template <typename T, std::size_t N> Array<T, N>::~Array() {
  helpers::printf("Array Dtor");
}

template <typename T, std::size_t N>
Array<T, N>::Array(const Array<T, N>& copy) : Array() {
  helpers::printf("Array Copy Ctor");

  for (std::size_t index{}; index < N; ++index) {
    _elements[index] = copy._elements[index];
  }
}

template <typename T, std::size_t N>
Array<T, N>& Array<T, N>::operator=(const Array<T, N>& copy) {
  helpers::printf("Array copy");

  for (std::size_t index{}; index < N; ++index) {
    _elements[index] = copy._elements[index];
  }
  return *this;
}

template <typename T, std::size_t N>
Array<T, N>::Array(Array<T, N>&& move) noexcept : Array() {
  helpers::printf("Array move Ctor");

  for (std::size_t index{}; index < N; ++index) {
    _elements[index] = std::move(move._elements[index]);
  }
}

template <typename T, std::size_t N>
Array<T, N>& Array<T, N>::operator=(Array<T, N>&& move) noexcept {
  helpers::printf("Array Move assignment");

  for (std::size_t index{}; index < N; ++index) {
    _elements[index] = std::move(move._elements[index]);
  }
  return *this;
}

/* Iterator */
template <typename T, std::size_t N>
template <bool IsConst>
class Array<T, N>::Iterator {
public:
  using iterator_category = std::contiguous_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = std::conditional_t<IsConst, const T, T>;
  using pointer = value_type*;
  using reference = value_type&;

private:
  pointer _current;

public:
  Iterator(pointer p) : _current{p} {}

  template <bool Q = IsConst>
  typename std::enable_if<Q, reference>::type operator*() const {
    return *_current;
  }

  template <bool Q = IsConst>
  typename std::enable_if<!Q, reference>::type operator*() {
    return *_current;
  }
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

template <typename T, std::size_t N> class Array<T, N>::ReverseIterator {
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

template <typename T, std::size_t N>
typename Array<T, N>::iterator Array<T, N>::begin() {
  return iterator{&_elements[0]};
}

template <typename T, std::size_t N>
typename Array<T, N>::const_iterator Array<T, N>::begin() const {
  return const_iterator{&_elements[0]};
}

template <typename T, std::size_t N>
typename Array<T, N>::const_iterator Array<T, N>::cbegin() const {
  return const_iterator{&_elements[0]};
}

template <typename T, std::size_t N>
typename Array<T, N>::iterator Array<T, N>::end() {
  return iterator{&_elements[N]};
}

template <typename T, std::size_t N>
typename Array<T, N>::const_iterator Array<T, N>::end() const {
  return const_iterator{&_elements[N]};
}

template <typename T, std::size_t N>
typename Array<T, N>::const_iterator Array<T, N>::cend() const {
  return const_iterator{&_elements[N]};
}

template <typename T, std::size_t N>
typename Array<T, N>::reverse_iterator Array<T, N>::rend() {
  return reverse_iterator{&_elements[0]};
}

template <typename T, std::size_t N>
typename Array<T, N>::reverse_iterator Array<T, N>::rend() const {
  return reverse_iterator{&_elements[0]};
}

template <typename T, std::size_t N>
typename Array<T, N>::reverse_iterator Array<T, N>::crend() const {
  return reverse_iterator{&_elements[0]};
}

template <typename T, std::size_t N>
typename Array<T, N>::reverse_iterator Array<T, N>::rbegin() {
  return reverse_iterator{&_elements[N]};
}

template <typename T, std::size_t N>
typename Array<T, N>::reverse_iterator Array<T, N>::rbegin() const {
  return reverse_iterator{&_elements[N]};
}

template <typename T, std::size_t N>
typename Array<T, N>::reverse_iterator Array<T, N>::crbegin() const {
  return reverse_iterator{&_elements[N]};
}