#include <algorithm.hpp>
#include <helpers.hpp>
#include <string_view>

/*
  Problem description
Given a list of parenthesis, Write a function to check whether that list is
  balanced
Ex:
INPUT: [{}{}()] => OUTPUT: true (balanced)
INPUT: [{] => OUTPUT: false (unbalanced)
INPUT: [{]} => OUTPUT: false (unbalanced)
*/
int main(int argc, char* argv[]) {
  if (argc > 1) {
    std::string_view parenthesis{argv[1]};
    if (algorithms::check_balanced_parenthesis(parenthesis)) {
      helpers::printf("BALANCED");
    } else {
      helpers::printf("UNBALANCED");
    }
  } else {
    helpers::printf("NO PARAM INSERTED");
  }
}