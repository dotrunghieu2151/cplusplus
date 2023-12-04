#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

struct Ingredient {
  std::string name{};
  double quantity{};
};

class IngredientsParser {
  std::ifstream _fileInput{};

public:
  IngredientsParser(std::string fullFilePath) { _fileInput.open(fullFilePath); }

  std::map<std::string, std::vector<Ingredient>> get_ingredient_dict() {
    std::string line{};
    std::map<std::string, std::vector<Ingredient>> ingredientDict{};
    while (std::getline(_fileInput, line)) {
      std::istringstream stream{line};
      std::string recipe;
      std::string ingredientName;
      double quantity;
      stream >> recipe >> ingredientName >> quantity;
      if (ingredientDict.find(recipe) != ingredientDict.end()) {
        ingredientDict[recipe].push_back(Ingredient{ingredientName, quantity});
      } else {
        std::vector<Ingredient> d{Ingredient{ingredientName, quantity}};
        ingredientDict.insert({recipe, d});
      }
    }
    return ingredientDict;
  }

  ~IngredientsParser() { _fileInput.close(); }
};

class OrderParser {
  std::ifstream _fileInput{};

public:
  OrderParser(std::string fullFilePath) { _fileInput.open(fullFilePath); }

  void process_order(
      const std::map<std::string, std::vector<Ingredient>>& ingredientDict) {
    std::string line{};
    std::map<std::string, double> orderDict{};
    while (std::getline(_fileInput, line)) {
      std::istringstream stream{line};
      std::string recipe;
      double quantity;
      stream >> recipe >> quantity;
      if (ingredientDict.find(recipe) != ingredientDict.end()) {
        const std::vector<Ingredient>& ingredients{ingredientDict.at(recipe)};
        for (const Ingredient& i : ingredients) {
          if (orderDict.find(i.name) != orderDict.end()) {
            orderDict[i.name] += i.quantity * quantity;
          } else {
            orderDict.insert({i.name, i.quantity * quantity});
          }
        }
      }
    }
    for (auto const& i : orderDict) {
      std::cout << i.first << " " << i.second // string's value
                << "\n";
    }
    return;
  }

  ~OrderParser() { _fileInput.close(); }
};
int main() {
  IngredientsParser ingredientParser{
      "/Users/kiaisoft/Projects/C++/learning c++/src/ingredients.txt"};
  OrderParser orderParser{
      "/Users/kiaisoft/Projects/C++/learning c++/src/orders.txt"};
  const std::map<std::string, std::vector<Ingredient>> ingredientDict{
      ingredientParser.get_ingredient_dict()};
  orderParser.process_order(ingredientDict);
}