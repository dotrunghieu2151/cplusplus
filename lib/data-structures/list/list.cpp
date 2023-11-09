#include <algorithm>
#include <cassert>
#include <cmath>
#include <concept.hpp>
#include <cstddef>
#include <helpers.hpp>
#include <initializer_list>
#include <iostream>
#include <iterator> // For std::forward_iterator_tag
#include <list.hpp>
#include <memory> //std::addressof
#include <new>    // for (T*)::operator new (sizeof(T)) ::operator delete(p)
#include <type_traits>
#include <utility>

template <typename T>
void List<T>::link_node_left(Node& newNode, Node& nodeToLinkTo) {
  newNode._next = &nodeToLinkTo;
  newNode._prev = nodeToLinkTo._prev;

  nodeToLinkTo._prev->_next = &newNode;
  nodeToLinkTo._prev = &newNode;
}

template <typename T>
void List<T>::link_node_right(Node& newNode, Node& nodeToLinkTo) {
  newNode._next = nodeToLinkTo._next;
  newNode._prev = &nodeToLinkTo;

  nodeToLinkTo._next->_prev = &newNode;
  nodeToLinkTo._next = &newNode;
}

template <typename T> void List<T>::unlink_node(Node& node) {
  node._prev->_next = node._next;
  node._next->_prev = node._prev;

  node._prev = nullptr;
  node._next = nullptr;
}

template <typename T> void List<T>::remove_node(Node* node) {
  unlink_node(*node);
  delete node;
  --_size;
};

template <typename T>
typename List<T>::Node* List<T>::reverse_recursive_foward(Node* node) noexcept {
  if (node->_next == end()) {
    _sentinal._next = node;
    return node;
  }

  Node* reversedNext{reverse_recursive_foward(node->_next)};
  reversedNext->_next = node;
  node->_prev = reversedNext;
  return node;
};

/* Constructors */
template <typename T> List<T>::List() : _sentinal{&_sentinal, &_sentinal} {};

template <typename T>
List<T>::List(std::size_t capacity, T defaultValue)
    : _size{capacity}, _sentinal{&_sentinal, &_sentinal} {
  helpers::printf("LIST CAPACITY DEFAULT CTOR");
  if (capacity <= 0) {
    throw OutOfRangeException("capacity should be greater than 0");
  }
  DataNode* temp{new DataNode{defaultValue}};
  link_node_right(*temp, _sentinal);
  for (std::size_t i{0}; i < _size - 1; ++i) {
    DataNode* newNode{new DataNode{defaultValue}};
    link_node_right(*newNode, *temp);
    temp = newNode;
  }
};

template <typename T> List<T>::List(std::initializer_list<T> list) : List() {
  helpers::printf("LIST LIST CTOR");
  insert(begin(), list.begin(), list.end());
};

template <typename T>
template <typename InputIterator>
List<T>::List(InputIterator begin, InputIterator end) : List() {
  helpers::printf("LIST ITERATOR CTOR");
  insert(List<T>::begin(), begin, end);
};

template <typename T> List<T>::~List() {
  helpers::printf("LIST DTOR");
  Node* next;
  for (Node* i = _sentinal._next; i != &_sentinal; i = next) {
    next = i->_next;
    delete i;
  }
};

template <typename T>
List<T>::List(const List<T>& copy) : List(copy.cbegin(), copy.cend()) {
  helpers::printf("LIST CP CTOR");
};

template <typename T> List<T>& List<T>::operator=(const List<T>& other) {
  helpers::printf("LIST CP OP");
  List<T> copy{other};
  copy.swap(*this);
  return *this;
};

template <typename T> List<T>::List(List<T>&& move) noexcept : List() {
  move.swap(*this);
};

template <typename T> List<T>& List<T>::operator=(List<T>&& move) noexcept {
  helpers::printf("LIST MOVE OP");
  move.swap(*this);
  return *this;
};

template <typename T> class List<T>::Iterator {
  friend class List<T>;

public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using reference = T&;

private:
  Node* _current{nullptr};
  Iterator() = default;
  Iterator(Node* p) : _current{p} {};

public:
  reference operator*() { return static_cast<DataNode*>(_current)->_data; };
  pointer operator->() { return &static_cast<DataNode*>(_current)->_data; };

  Iterator& operator++() {
    _current = _current->_next;
    return *this;
  };

  Iterator operator++(int) {
    Iterator tmp{*this};
    ++(*this);
    return tmp;
  }

  Iterator& operator--() {
    _current = _current->_prev;
    return *this;
  }

  Iterator operator--(int) {
    Iterator tmp{*this};
    --(*this);
    return tmp;
  }

  bool operator==(const Iterator& other) const {
    return _current == other._current;
  }

  bool operator!=(const Iterator& other) const { return !(*this == other); }
};

template <typename T> class List<T>::ReverseIterator {
  friend class List<T>;

public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using reference = T&;

private:
  Node* _current{nullptr};
  ReverseIterator() = default;
  ReverseIterator(Node* p) : _current{p} {};

public:
  reference operator*() {
    return static_cast<DataNode*>(_current->_prev)->_data;
  }
  pointer operator->() {
    return &static_cast<DataNode*>(_current->_prev)->_data;
  }

  ReverseIterator& operator++() {
    _current = _current->_prev;
    return *this;
  }

  ReverseIterator operator++(int) {
    ReverseIterator tmp{*this};
    ++(*this);
    return tmp;
  }

  ReverseIterator& operator--() {
    _current = _current->_next;
    return *this;
  }

  ReverseIterator operator--(int) {
    ReverseIterator tmp{*this};
    --(*this);
    return tmp;
  }

  bool operator==(const ReverseIterator& other) const {
    return _current == other._current;
  }

  bool operator!=(const ReverseIterator& other) const {
    return !(*this == other);
  }
};

template <typename T> typename List<T>::iterator List<T>::begin() {
  return iterator{_sentinal._next};
}

template <typename T> typename List<T>::iterator List<T>::begin() const {
  return iterator{_sentinal._next};
}

template <typename T> typename List<T>::iterator List<T>::cbegin() const {
  return iterator{_sentinal._next};
}

template <typename T> typename List<T>::iterator List<T>::end() {
  return iterator{&_sentinal};
}

template <typename T> typename List<T>::iterator List<T>::end() const {
  return iterator{const_cast<Node*>(&_sentinal)};
}

template <typename T> typename List<T>::iterator List<T>::cend() const {
  return iterator{const_cast<Node*>(&_sentinal)};
}

template <typename T> typename List<T>::reverse_iterator List<T>::rbegin() {
  return reverse_iterator{end()._current};
}

template <typename T>
typename List<T>::reverse_iterator List<T>::rbegin() const {
  return reverse_iterator{end()._current};
}

template <typename T>
typename List<T>::reverse_iterator List<T>::crbegin() const {
  return reverse_iterator{end()._current};
}

template <typename T> typename List<T>::reverse_iterator List<T>::rend() {
  return reverse_iterator{begin()._current};
}

template <typename T> typename List<T>::reverse_iterator List<T>::rend() const {
  return reverse_iterator{begin()._current};
}

template <typename T>
typename List<T>::reverse_iterator List<T>::crend() const {
  return reverse_iterator{begin()._current};
}

template <typename T> void List<T>::push_back(const T& element) {
  insert(end(), element);
}

template <typename T> void List<T>::push_back(T&& element) {
  insert(end(), std::move(element));
}

template <typename T> void List<T>::push_front(const T& element) {
  insert(begin(), element);
}

template <typename T> void List<T>::push_front(T&& element) {
  insert(begin(), std::move(element));
}

template <typename T> void List<T>::pop_back() { remove_node(_sentinal._prev); }

template <typename T> void List<T>::pop_front() {
  remove_node(_sentinal._next);
}

template <typename T>
typename List<T>::iterator List<T>::insert(iterator pos, const T& element) {
  DataNode* newNode{new DataNode{element}};
  link_node_left(*newNode, *pos._current);
  ++_size;
  return iterator{newNode};
}

template <typename T>
typename List<T>::iterator List<T>::insert(iterator pos, T&& element) {
  DataNode* newNode{new DataNode{std::move(element)}};
  link_node_left(*newNode, *pos._current);
  ++_size;
  return iterator{newNode};
}

template <typename T>
template <typename InputIterator>
typename List<T>::iterator List<T>::insert(iterator pos, InputIterator begin,
                                           InputIterator end) {
  iterator previousIter{--pos};
  for (auto i{begin}; i != end; ++i) {
    DataNode* newNode{new DataNode{*i}};
    link_node_right(*newNode, *previousIter._current);
    ++_size;
    previousIter = iterator{newNode};
  }

  return previousIter;
}

template <typename T> typename List<T>::iterator List<T>::erase(iterator pos) {
  if (pos == end()) {
    return end();
  }
  iterator next{pos._current->_next};
  remove_node(pos._current);
  return next;
}

template <typename T>
typename List<T>::iterator List<T>::erase(iterator first, iterator last) {
  for (iterator i{first}; i != last;) {
    i = erase(i);
  }
  return last;
}

template <typename T> void List<T>::reverse_iter() noexcept {
  Node* previous{&_sentinal};
  for (Node* ptr{_sentinal._next}; ptr != end();) {
    Node* temp{ptr->_next};
    ptr->_next = previous;
    ptr->_prev = temp;
    previous = ptr;
    ptr = temp;
  }

  _sentinal._prev = _sentinal._next;
  _sentinal._next = previous;
}

template <typename T> void List<T>::reverse_recursive() noexcept {
  reverse_recursive_foward(&_sentinal);
}