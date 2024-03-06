#include <iostream>
#include <monster-generator.hpp>
#include <monster.hpp>

int main() {
  using namespace std::literals;

  Monster monster{MonsterGenerator::generate()};
  monster.print();
}