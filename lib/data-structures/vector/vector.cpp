#include <algorithm>
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
#include <vector.hpp>

/* Constructors */
using std::cout;
using std::endl;

#define VECTOR_DEBUG 1

#if VECTOR_DEBUG == 1
#define VECTOR_DEBUG_MS(mes)                                                   \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define VECTOR_DEBUG_MS(mes)                                                   \
  do {                                                                         \
  } while (0)
#endif

template <typename T>
Vector<T>::Vector(std::size_t capacity)
    : _size{0}, _space{capacity},
      _elements{static_cast<T*>(::operator new(sizeof(T) * capacity))} {
  VECTOR_DEBUG_MS("Vector Ctor capacity");
}

template <typename T>
Vector<T>::Vector(std::initializer_list<T> list) : Vector(list.size() + 2) {
  VECTOR_DEBUG_MS("Vector Ctor List");

  for (std::size_t index{}; index < list.size(); ++index) {
    new (_elements + index) T{*(list.begin() + index)};
  }
  _size = list.size();
}

template <typename T>
template <std::random_access_iterator Iter>
Vector<T>::Vector(Iter begin, Iter end)
    : Vector(static_cast<std::size_t>(end - begin)) {
  VECTOR_DEBUG_MS("Vector Iterator Ctor");
  for (auto i{begin}; i != end; ++i) {
    push_back(*i);
  }
}

template <typename T> Vector<T>::~Vector() {
  VECTOR_DEBUG_MS("Vector Dtor");

  clear();
  ::operator delete(_elements);
}

template <typename T>
Vector<T>::Vector(const Vector<T>& copy)
    : _size{copy._size}, _space{copy._space},
      _elements{static_cast<T*>(::operator new(sizeof(T) * copy._space))} {
  VECTOR_DEBUG_MS("Vector Copy Ctor");

  for (std::size_t index{}; index < _size; ++index) {
    new (_elements + index) T{copy._elements[index]};
  }
}

// copy-and-swap and move-and-swap idiom
// inefficient, need testing
template <typename T> Vector<T>& Vector<T>::operator=(const Vector<T>& copy) {
  VECTOR_DEBUG_MS("Vector copy assign capacity");

  Vector<T> tempCopy{copy};
  tempCopy.swap(*this);
  return *this;
}

template <typename T>
Vector<T>::Vector(Vector<T>&& move) noexcept
    : _size{move._size}, _space{move._space}, _elements{move._elements} {
  VECTOR_DEBUG_MS("Move Ctor capacity");

  move._size = 0;
  move._space = 0;
  move._elements = nullptr;
}

template <typename T>
Vector<T>& Vector<T>::operator=(Vector<T>&& move) noexcept {
  VECTOR_DEBUG_MS("Move assignment capacity");

  Vector<T> tempMove{std::move(move)};
  tempMove.swap(*this);
  return *this;
}

/* Iterator */
template <typename T> class Vector<T>::Iterator {
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

template <typename T> typename Vector<T>::Iterator Vector<T>::begin() {
  return Vector<T>::Iterator{&_elements[0]};
}

template <typename T>
const typename Vector<T>::Iterator Vector<T>::cbegin() const {
  return Vector<T>::Iterator{&_elements[0]};
}

template <typename T> typename Vector<T>::Iterator Vector<T>::end() {
  return Vector<T>::Iterator{&_elements[_size]};
}

template <typename T>
const typename Vector<T>::Iterator Vector<T>::cend() const {
  return Vector<T>::Iterator{&_elements[_size]};
}

/* Capacity */
template <typename T> void Vector<T>::reserve(std::size_t newCapacity) {
  if (newCapacity > _space) {
    reallocate(newCapacity);
  }
}

template <typename T>
void Vector<T>::resize(std::size_t newsize, const T& val) {
  if (_size > newsize) {
    for (std::size_t index = newsize; index < _size; ++index) {
      _elements[index].~T();
    }
  } else if (_size < newsize) {
    reserve(newsize);
    for (std::size_t index = _size; index < newsize; ++index) {
      new (_elements + index) T{val};
    }
  }

  _size = newsize;
}

template <typename T> void Vector<T>::shrink_to_fit() { reallocate(_size); }

/* Modifiers */
template <typename T> void Vector<T>::clear() {
  while (_size) {
    pop_back();
  }
}
template <typename T> void Vector<T>::push_back(T&& val) {
  reallocateIfRequired();
  new (_elements + _size) T{std::move(val)};
  ++_size;
}

template <typename T> void Vector<T>::push_back(const T& val) {
  reallocateIfRequired();
  new (_elements + _size) T{val};
  ++_size;
}
template <typename T> void Vector<T>::reallocateIfRequired() {
  if (_size < _space)
    return;
  std::size_t newCapacity{
      static_cast<std::size_t>(std::max(2.0, std::round(_space * growRate)))};
  reallocate(newCapacity);
}
template <typename T> void Vector<T>::reallocate(std::size_t capacity) {
  T* newArr{static_cast<T*>(::operator new(sizeof(T) * capacity))};

  for (std::size_t i = 0; i < _size; ++i) {
    new (newArr + i) T{std::move(_elements[i])};
  }

  _space = capacity;
  std::swap(_elements, newArr);

  ::operator delete(newArr);
}
template <typename T> T Vector<T>::pop_back() {
  T ele{std::move(_elements[--_size])};
  return ele;
}

template <typename T>
template <typename... Args>
void Vector<T>::emplace_back(Args&&... args) {
  reallocateIfRequired();
  new (_elements + _size) T{std::forward<Args>(args)...};
  ++_size;
}

template <typename T> void Vector<T>::erase(std::size_t index) {
  validateIndex(index);
  _elements[index].~T();
  for (std::size_t i{index}; i < _size; ++i) {
    new (_elements + i) T{std::move(_elements[i + 1])};
    _elements[i + 1].~T();
  }
  --_size;
}

template <typename T>
void Vector<T>::erase(std::size_t startIndex, std::size_t endIndex) {
  validateIndex(startIndex);
  validateIndex(endIndex);
  std::size_t distance{endIndex - startIndex + 1};
  for (std::size_t i{startIndex}; i <= endIndex; ++i) {
    _elements[i].~T();
  }
  for (std::size_t i{endIndex + 1}; i < _size; ++i) {
    new (_elements + i - distance) T{std::move(_elements[i])};
    _elements[i].~T();
  }
  _size -= distance;
}

template <typename T>
void Vector<T>::insert(std::size_t insertIndex, const T& ele) {
  validateIndex(insertIndex);
  reallocateIfRequired();
  ++_size;
  for (std::size_t i{_size - 1}; i > insertIndex; --i) {
    new (_elements + i) T{std::move(_elements[i - 1])};
    _elements[i - 1].~T();
  }
  new (_elements + insertIndex) T{ele};
}
template <typename T> void Vector<T>::insert(std::size_t insertIndex, T&& ele) {
  validateIndex(insertIndex);
  reallocateIfRequired();
  ++_size;
  for (std::size_t i{_size - 1}; i > insertIndex; --i) {
    new (_elements + i) T{std::move(_elements[i - 1])};
    _elements[i - 1].~T();
  }
  new (_elements + insertIndex) T{std::move(ele)};
}

template <typename T>
template <typename... Args>
void Vector<T>::insert(std::size_t insertIndex, Args&&... args) {
  validateIndex(insertIndex);
  constexpr int eleNum = sizeof...(args);
  std::size_t oldSize{_size};
  _size += eleNum;
  if (_size > _space) {
    reallocate(_size * growRate);
  }

  for (std::size_t i{oldSize - 1}; i >= insertIndex; --i) {
    new (_elements + i + eleNum) T{std::move(_elements[i])};
    _elements[i].~T();
  }
  ((new (_elements + (insertIndex++)) T{std::forward<T>(args)}), ...);
}

template <typename T>
template <concepts::IsIterator Input>
void Vector<T>::insert(std::size_t insertIndex, Input first, Input last) {
  validateIndex(insertIndex);
  std::size_t eleNum{static_cast<std::size_t>(last - first)};
  std::size_t oldSize{_size};
  _size += eleNum;
  if (_size > _space) {
    reallocate(_size * growRate);
  }

  for (std::size_t i{oldSize - 1}; i >= insertIndex; --i) {
    new (_elements + i + eleNum) T{std::move(_elements[i])};
    _elements[i].~T();
  }
  for (auto index = first; index != last; ++index) {
    new (_elements + (insertIndex++)) T{*index};
  }
}
