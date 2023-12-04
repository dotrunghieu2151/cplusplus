#pragma once

#include <cstddef>
#include <limits>
#include <new>
#include <utility>

template <typename T> class Allocator {

public:
  using value_type = T;

  using pointer = value_type*;
  using const_pointer = const pointer;

  using reference = value_type&;
  using const_reference = const value_type&;
  using rvalue_reference = value_type&&;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  template <typename U> struct rebind {
    using other = Allocator<U>;
  };

  Allocator(){};
  ~Allocator(){};
  Allocator(const Allocator&){};
  template <typename U> Allocator(const Allocator<U>&) {}

  // address
  pointer address(reference r) { return &r; };
  pointer address(const_reference r) const { return &r; };

  // memory allocation
  pointer allocate(size_type count) {
    return reinterpret_cast<pointer>(
        ::operator new(count * sizeof(value_type)));
  };
  void deallocate(pointer p, size_type = 0) { ::operator delete(p); };

  // ctor / detor
  void construct(pointer p, const_reference t) { new (p) value_type{t}; };
  void construct(pointer p, rvalue_reference t) {
    new (p) value_type{std::move(t)};
  };
  void destruct(pointer p) { p->~value_type(); };

  bool operator==(Allocator const&) { return true; };
  bool operator!=(Allocator const& a) { return !(*this == a); };

  size_type max_size() const {
    return std::numeric_limits<size_type>::max() / sizeof(value_type);
  }
};