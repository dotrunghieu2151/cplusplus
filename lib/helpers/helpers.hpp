#pragma once
#include "iostream"

namespace helpers {
using std::cout;
using std::endl;

// use this function to force constexpr function to be compile-time run
consteval auto compileTime(auto value) { return value; }

template <typename... Args> void printf(Args&... args) {
  ((cout << args << ", "), ...) << endl;
}
template <typename Last> void printf(Last& last) { cout << last << endl; }

class Test {
private:
  int _num{};
  int* _ptr{};

public:
  Test() = default;
  Test(int num) : _num{num}, _ptr{new int{5}} {
    helpers::printf("Constructor called");
  }
  ~Test() {
    helpers::printf("Destructor called");
    delete _ptr;
    _ptr = nullptr;
  }

  Test(const Test& copy) : _num{copy._num} {
    helpers::printf("COPY Ctor");
    if (copy._ptr) {
      _ptr = new int{*copy._ptr};
    }
  }

  Test& operator=(const Test& copy) {
    helpers::printf("COPY assignment");
    Test tempCopy{copy};
    _num = copy._num;
    std::swap(_ptr, tempCopy._ptr);
    return *this;
  }

  Test(Test&& move) : _num{move._num}, _ptr{move._ptr} {
    helpers::printf("MOVE Ctor");
    move._ptr = nullptr;
  }

  Test& operator=(Test&& move) {
    helpers::printf("MOVE ASSIGNMENT");
    Test tmpMove{std::move(move)};
    std::swap(_ptr, tmpMove._ptr);
    std::swap(_num, tmpMove._num);
    return *this;
  }

  int num() const { return _num; }
};
} // namespace helpers