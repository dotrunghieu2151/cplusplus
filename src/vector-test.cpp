#include <helpers.hpp>
#include <iostream>
#include <iterator>
#include <list.hpp>
#include <utility>
#include <vector.hpp>

using helpers::Test;
using std::cout;
using std::endl;

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