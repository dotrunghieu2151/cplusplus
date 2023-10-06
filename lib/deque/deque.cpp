#include <algorithm>
#include <array.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <deque.hpp>
#include <helpers.hpp>
#include <initializer_list>
#include <iostream>
#include <iterator> // For std::forward_iterator_tag
#include <new>      // for (T*)::operator new (sizeof(T)) ::operator delete(p)
#include <type_traits>
#include <utility>
#include <vector.hpp>

/* Constructors */
using std::cout;
using std::endl;

template <typename T>
Deque<T>::Deque(std::size_t count, const_reference defaultValue)
    : _size{count}, _map{get_nodes_count(count)} {
  for (std::size_t i{0}; i < _map.size(); ++i) {
    _map[i] = buffer{defautValue};
  }
  helpers::printf("Deque Ctor default");
}

template <typename T>
Deque<T>::Deque(std::initializer_list<T> list)
    : _size{list.size()}, _map{get_nodes_count(list.size())} {
  helpers::printf("Deque Ctor List");

  for (std::size_t index{0}; index < list.size(); ++index) {
    _elements[index] = *(list.begin() + index);
  }
}

template <typename T> Deque<T>::~Deque() {
  helpers::printf("Array Dtor");
  delete[] _elements;
}

template <typename T>
Deque<T>::Deque(const Deque<T>& copy) : _size{copy._size}, _elements{new T[N]} {
  helpers::printf("Array Copy Ctor");

  for (std::size_t index = 0; index < _size; ++index) {
    _elements[index] = copy._elements[index];
  }
}

template <typename T> Deque<T>& Deque<T>::operator=(const Deque<T>& copy) {
  helpers::printf("Array copy");

  Deque<T> temp{copy};
  temp.swap(*this);
  return *this;
}

template <typename T>
Deque<T>::Deque(Deque<T>&& move) noexcept
    : _size{move._size}, _elements{move._elements} {
  helpers::printf("Array move Ctor");

  move._size = 0;
  move._elements = nullptr;
}

template <typename T> Deque<T>& Deque<T>::operator=(Deque<T>&& move) noexcept {
  helpers::printf("Array Move assignment");

  Deque<T> tempMove{std::move(move)};
  tempMove.swap(*this);
  return *this;
}

/* Iterator */
template <typename T> class Deque<T>::Iterator {
public:
  using iterator_category = std::contiguous_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using reference = T&;
  using const_reference = const T&;

private:
  pointer _current;
  pointer _start;
  pointer _end;
  pointer* map_ptr;

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

template <typename T> class Deque<T>::ReverseIterator {
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

template <typename T> typename Deque<T>::iterator Deque<T>::begin() {
  return iterator{&_elements[0]};
}

template <typename T> typename Deque<T>::iterator Deque<T>::begin() const {
  return iterator{&_elements[0]};
}

template <typename T> typename Deque<T>::iterator Deque<T>::cbegin() const {
  return iterator{&_elements[0]};
}

template <typename T> typename Deque<T>::iterator Deque<T>::end() {
  return iterator{&_elements[_size]};
}

template <typename T> typename Deque<T>::iterator Deque<T>::end() const {
  return iterator{&_elements[_size]};
}

template <typename T> typename Deque<T>::iterator Deque<T>::cend() const {
  return iterator{&_elements[_size]};
}

template <typename T> typename Deque<T>::reverse_iterator Deque<T>::rend() {
  return reverse_iterator{&_elements[0]};
}

template <typename T>
typename Deque<T>::reverse_iterator Deque<T>::rend() const {
  return reverse_iterator{&_elements[0]};
}

template <typename T>
typename Deque<T>::reverse_iterator Deque<T>::crend() const {
  return reverse_iterator{&_elements[0]};
}

template <typename T> typename Deque<T>::reverse_iterator Deque<T>::rbegin() {
  return reverse_iterator{&_elements[_size]};
}

template <typename T>
typename Deque<T>::reverse_iterator Deque<T>::rbegin() const {
  return reverse_iterator{&_elements[_size]};
}

template <typename T>
typename Deque<T>::reverse_iterator Deque<T>::crbegin() const {
  return reverse_iterator{&_elements[_size]};
}