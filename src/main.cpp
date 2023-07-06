#include <algorithm>
#include <array>
#include <enums.hpp>
#include <iostream>
#include <log.hpp>
#include <math.constant.hpp>
#include <math.hpp>
#include <random.hpp>
#include <string>
#include <structs.hpp>
#include <timer.hpp>
#include <utilities.hpp>
#include <vector>

// Function-like macro that evaluates to true if the type (or object) uses 2 or
// fewer memory addresses worth of memory
#define isSmall(T) (sizeof(T) <= 2 * sizeof(void*))

int main() {
  using math::isFloatingPointEqual;
  using std::cout;
  int var = 8;
  const int* const voidptr = &var;
  int rndNumber = Random::uniformRand(1, 20);
  Fraction<int, std::string> intFraction;
  TestScoped testEnum{1};
  testEnum = TestScoped::test3;
  const std::size_t length{100};
  int arr[length]{};
  std::array<int, length> arr1{};
  std::vector<int> arr2(length);
  std::size_t randIndex{
      static_cast<std::size_t>(Random::uniformRand(0, length - 1))};
  arr[randIndex] = 2;
  arr1.at(randIndex) = 4;
  arr2.at(randIndex) = 10;
  Timer t{};
  utilities::array::bubble_sort(std::begin(arr), std::end(arr));
  cout << std::fixed << "TIME arr1: " << t.elapsed() << endl;
  t.reset();
  std::sort(arr1.begin(), arr1.end());
  cout << std::fixed << "TIME arr2: " << t.elapsed() << endl;
  t.reset();
  utilities::array::bubble_sort(arr2.begin(), arr2.end());
  cout << std::fixed << "TIME arr3: " << t.elapsed() << endl;
  t.reset();
  auto found{
      utilities::array::binary_search(std::begin(arr), std::end(arr), 2)};
  cout << std::fixed << "TIME search arr1: " << t.elapsed() << endl;
  t.reset();
  auto foundArr1{utilities::array::binary_search(arr1.begin(), arr1.end(), 4)};
  cout << std::fixed << "TIME search arr2: " << t.elapsed() << endl;
  t.reset();
  auto foundArr2{utilities::array::binary_search(arr2.begin(), arr2.end(), 10,
                                                 [](auto a, auto b) {
                                                   if (*a == b)
                                                     return 0;
                                                   else if (*a < b)
                                                     return -1;
                                                   else
                                                     return 1;
                                                 })};
  cout << std::fixed << "TIME search arr3: " << t.elapsed() << endl;
  t.reset();
  cout << *voidptr << endl;
  cout << constants::math::gravity << endl;
  cout << isFloatingPointEqual(0.1, 0.1) << endl;
  cout << "Random number:" << rndNumber << endl;
  cout << "max: " << math::max(0.4, 2.6) << endl;
  cout << "min: " << math::min(3, 3.75) << endl;
  // enums
  cout << "enum unscoped: " << test2 << endl;
  cout << "enum scoped: " << static_cast<int>(testEnum) << endl;
  cout << "foundArr: " << *found << endl;

  cout << "foundArr1: " << *foundArr1 << endl;
  cout << "foundArr2: " << *foundArr2 << endl;
}