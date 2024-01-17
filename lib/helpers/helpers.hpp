#pragma once
#include <iostream>

#define HELPER_DEBUG 1

#if HELPER_DEBUG == 1
#define HELPER_DEBUG_MS(mes)                                                   \
  do {                                                                         \
    helpers::printf(mes);                                                      \
  } while (0)
#else
#define HELPER_DEBUG_MS(mes)                                                   \
  do {                                                                         \
  } while (0)
#endif
namespace helpers {
using std::cout;
using std::endl;

// use this function to force constexpr function to be compile-time run
consteval auto compileTime(auto value) { return value; }

template <typename... Args> void printf(Args&... args) {
  ((cout << args << ", "), ...) << "\n";
}
template <typename Last> void printf(Last& last) { cout << last << "\n"; }

class Test {
private:
  int _num{};
  int* _ptr{nullptr};

public:
  Test() = default;
  Test(int num) : _num{num}, _ptr{new int{5}} {
    HELPER_DEBUG_MS("Constructor called");
  }
  ~Test() {
    HELPER_DEBUG_MS("Destructor called");
    if (_ptr) {
      delete _ptr;
      _ptr = nullptr;
    }
  }

  Test(const Test& copy) : _num{copy._num} {
    HELPER_DEBUG_MS("COPY Ctor");
    if (copy._ptr) {
      _ptr = new int{*copy._ptr};
    }
  }

  Test& operator=(const Test& copy) {
    HELPER_DEBUG_MS("COPY assignment");
    Test tempCopy{copy};
    _num = copy._num;
    std::swap(_ptr, tempCopy._ptr);
    return *this;
  }

  Test(Test&& move) : _num{move._num}, _ptr{move._ptr} {
    HELPER_DEBUG_MS("MOVE Ctor");
    move._ptr = nullptr;
  }

  Test& operator=(Test&& move) {
    HELPER_DEBUG_MS("MOVE ASSIGNMENT");
    Test tmpMove{std::move(move)};
    std::swap(_ptr, tmpMove._ptr);
    std::swap(_num, tmpMove._num);
    return *this;
  }

  friend bool operator==(const Test& o1, const Test& o2) {
    return o1._num == o2._num;
  };

  friend bool operator!=(const Test& o1, const Test& o2) {
    return !(operator==(o1, o2));
  };

  friend bool operator<(const Test& o1, const Test& o2) {
    return o1._num < o2._num;
  };

  friend bool operator>(const Test& o1, const Test& o2) {
    return operator<(o2, o1);
  };

  friend bool operator<=(const Test& o1, const Test& o2) {
    return !operator>(o1, o2);
  };

  friend bool operator>=(const Test& o1, const Test& o2) {
    return !operator<(o1, o2);
  };

  friend std::ostream& operator<<(std::ostream& os, const Test& test) {
    return os << test.num();
  }

  int num() const { return _num; }

  void setNum(int num) { _num = num; }
};
} // namespace helpers