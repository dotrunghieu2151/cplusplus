#pragma once

#include <concept.hpp>
#include <cstddef>
#include <deque.hpp>
#include <helpers.hpp>
#include <string>
#include <utility>

template <typename T, concepts::IsStackContainer Container = Deque<T>>
class Stack {

public:
  using container_type = Container;
  using value_type = typename Container::value_type;
  using reference = typename Container::reference;
  using rvalue_reference = typename Container::rvalue_reference;

private:
  using self = Stack<T, Container>;
  container_type _container{};

public:
  /* Constructors */
  Stack() = default;
  Stack(const container_type& container) : _container{container} {};
  Stack(container_type&& container) : _container{std::move(container)} {};

  ~Stack() = default;

  Stack(const self& copy) : Stack(copy._container){};
  self& operator=(const self& copy) {
    self tmp{copy};
    tmp.swap(*this);
    return *this;
  };

  Stack(self&& move) noexcept : Stack(std::move(move._container)){};
  self& operator=(self&& move) noexcept {
    self tmp{std::move(move)};
    tmp.swap(*this);
    return *this;
  };

  reference top() noexcept { return _container.back(); }

  reference top() const noexcept { return _container.back(); }

  bool empty() const noexcept { return _container.empty(); }

  std::size_t size() const noexcept { return _container.size(); }

  void swap(self& other) noexcept {
    using std::swap;
    swap(_container, other._container);
  }

  void friend swap(self& o1, self& o2) noexcept { return o1.swap(o2); }

  void push(const reference ele) { _container.push_back(ele); }

  void push(rvalue_reference ele) { _container.push_back(std::move(ele)); }

  value_type pop() { return _container.pop_back(); }
};