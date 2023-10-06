#pragma once

#include <concept.hpp>
#include <cstddef>
#include <helpers.hpp>
#include <initializer_list>
#include <string>
#include <utility>

template <typename T> class List {

  class OutOfRangeException : public std::exception {
  private:
    std::string message;

  public:
    OutOfRangeException(std::string msg) : message{msg} {}
  };

private:
  struct Node {
  public:
    Node* _next{nullptr};
    Node* _prev{nullptr};

    Node() = default;
    Node(Node* prev, Node* next) : _next{next}, _prev{prev} {
      _next->_prev = this;
      _prev->_next = this;
    };

    virtual ~Node() = default;
  };

  struct DataNode : public Node {

  public:
    T _data{};

    DataNode() = default;
    DataNode(const T& data) : _data{data} {};
    DataNode(T&& data) : _data{std::move(data)} {};
  };

private:
  std::size_t _size{};
  // _sentinal._next is head, _sentinal._prev is tail
  Node _sentinal{};

  void link_node_right(Node& newNode, Node& nodeToLinkTo);
  void link_node_left(Node& newNode, Node& nodeToLinkTo);
  void unlink_node(Node& node);
  void remove_node(Node* node);
  T& get_node_at_index(std::size_t index) const {
    if (index == _size - 1) {
      return static_cast<DataNode*>(_sentinal._prev)->_data;
    } else if (index == 0) {
      return static_cast<DataNode*>(_sentinal._next)->_data;
    } else {
      Node* tmp{_sentinal._next};
      for (std::size_t i{0}; i < index; ++i) {
        tmp = tmp->_next;
      };
      return static_cast<DataNode*>(tmp)->_data;
    }
  };
  Node* reverse_recursive_foward(Node* node) noexcept;

public:
  /* Constructors */
  List();
  List(std::size_t capacity, T defaultValue = T());

  template <typename InputIterator>
  List(InputIterator begin, InputIterator end);
  List(std::initializer_list<T> list);

  ~List();

  List(const List<T>& copy);
  List<T>& operator=(const List<T>& copy);

  List(List<T>&& move) noexcept;
  List<T>& operator=(List<T>&& move) noexcept;

  /* Element access */
  T& operator[](std::size_t index) { return get_node_at_index(index); };
  const T& operator[](std::size_t index) const {
    return get_node_at_index(index);
  };

  T& at(std::size_t index) {
    if (index > _size) {
      throw OutOfRangeException("Index out of range");
    }
    return get_node_at_index(index);
  };

  const T& at(std::size_t index) const {
    if (index > _size) {
      throw OutOfRangeException("Index out of range");
    }
    return get_node_at_index(index);
  };

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

  void swap(List<T>& copy) noexcept {
    using std::swap;
    // order of execution is important to break the link, dont change the order
    swap(_sentinal._next->_prev, copy._sentinal._next->_prev);
    swap(_sentinal._next, copy._sentinal._next);

    swap(_sentinal._prev->_next, copy._sentinal._prev->_next);
    swap(_sentinal._prev, copy._sentinal._prev);

    swap(_size, copy._size);
  }
  friend void swap(List<T>& a, List<T>& b) noexcept { a.swap(b); };

  std::size_t size() const { return _size; }

  void pop_back();
  void pop_front();

  void push_back(const T& element);
  void push_back(T&& element);
  void push_front(const T& element);
  void push_front(T&& element);

  iterator insert(iterator pos, const T& element);
  iterator insert(iterator pos, T&& element);

  template <typename InputIterator>
  iterator insert(iterator pos, InputIterator begin, InputIterator end);

  iterator erase(iterator pos);
  iterator erase(iterator first, iterator last);

  void reverse_iter() noexcept;
  void reverse_recursive() noexcept;
};

#include <list.cpp>