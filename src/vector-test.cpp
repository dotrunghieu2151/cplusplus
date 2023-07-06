#include <helpers.hpp>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector.cpp>
#include <vector.hpp>

using std::cout;
using std::endl;

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

  int num() { return _num; }
};

int main() {
  Vector<Test> vec1(0);
  vec1.push_back(Test{1});
  vec1.push_back(Test{2});
  vec1.push_back(Test{3});
  vec1.push_back(Test{4});
  vec1.push_back(Test{5});
  vec1.push_back(Test{6});
  vec1.resize(3);
  vec1.push_back(Test{7});
  vec1.push_back(Test{8});
  vec1.push_back(Test{9});
  vec1.reserve(12);
  vec1.resize(2);
  vec1.emplace_back(10);
  vec1.shrink_to_fit();
  helpers::printf("BEGIN INSERTING");
  Vector<Test> vec2{Test{11}, Test{12}, Test{13}, Test{15}, Test{16}};
  vec1.insert(1, std::make_move_iterator(vec2.begin()),
              std::make_move_iterator(vec2.end()));
  vec1.pop_back();
  vec1.insert(vec1.size(), Test{14});
  vec1.erase(1, 5);
  vec1.insert(1, Test{19}, Test{20});
  for (Test& test : vec1) {
    cout << "ELEMENTS:" << test.num() << endl;
  }
  cout << "CAPACITY: " << vec1.capacity() << endl;
}