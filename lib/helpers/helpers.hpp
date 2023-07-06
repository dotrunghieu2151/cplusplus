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
} // namespace helpers