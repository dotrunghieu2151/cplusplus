#include <algorithm>
#include <array>
#include <stack.hpp>
#include <string>
#include <string_view>

namespace {
constexpr std::array openParenthesis{'[', '{', '('};
constexpr std::array closeParenthesis{']', '}', ')'};
constexpr std::array operators{'+', '-', '*', '/', '^'};

bool is_open_parenthesis(char character) {
  return std::find(std::begin(openParenthesis), std::end(openParenthesis),
                   character) != std::end(openParenthesis);
}

bool is_close_parenthesis(char c) {
  return std::find(std::begin(closeParenthesis), std::end(closeParenthesis),
                   c) != std::end(closeParenthesis);
}

bool are_parentheses_matched(char openingParenthesis, char closingParenthesis) {
  ptrdiff_t pointerIndex{std::find(std::begin(closeParenthesis),
                                   std::end(closeParenthesis),
                                   closingParenthesis) -
                         std::begin(closeParenthesis)};
  return pointerIndex < closeParenthesis.size() &&
         openingParenthesis == openParenthesis[pointerIndex];
}

bool is_numeric(char c) { return c >= '0' && c <= '9'; }

bool is_operand(char c) {
  return is_numeric(c) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Function to get weight of an operator. An operator with higher weight will
// have higher precedence.
int get_operator_weight(char op) {
  int weight = -1;
  switch (op) {
  case '+':
  case '-': {
    weight = 1;
    break;
  }
  case '*':
  case '/': {
    weight = 2;
    break;
  }
  case '^': {
    weight = 3;
    break;
  }
  }
  return weight;
}

int has_lower_precedence(char op1, char op2) {
  return get_operator_weight(op1) < get_operator_weight(op2);
}

bool is_operator(char c) {
  return std::find(std::begin(operators), std::end(operators), c) !=
         std::end(operators);
}

int perform_operation(char operation, int operand1, int operand2) {
  if (operation == '+')
    return operand1 + operand2;
  else if (operation == '-')
    return operand1 - operand2;
  else if (operation == '*')
    return operand1 * operand2;
  else if (operation == '/')
    return operand1 / operand2;
  else if (operation == '^')
    return std::pow(operand1, operand2);

  else
    cout << "perform_operation unexpected Error: Operator not found \n";

  return -1;
}

int evaluate_postfix(std::string_view str, char deliminator = ' ') {
  Stack<int> stack{};
  for (int i{}; i < str.length(); ++i) {
    char c{str[i]};
    if (is_numeric(c)) {
      int num{c - '0'};
      ++i;
      while (str[i] != deliminator) {
        num = num * 10 + (str[i] - '0');
        ++i;
      }
      stack.push(num);
    } else if (is_operator(c)) {
      int operand2{stack.pop()};
      int operand1{stack.pop()};
      stack.push(perform_operation(c, operand1, operand2));
    }
  }
  return stack.pop();
}

std::string convert_infix_to_postfix(std::string_view str, char deliminator) {
  Stack<char> stack{};
  std::string postfix{};
  postfix.reserve(str.size());
  for (int i{}; i < str.length(); ++i) {
    char c{str[i]};
    if (is_operand(c)) {
      // get multi-digit number
      postfix += c;
      ++i;
      while (is_operand(str[i])) {
        postfix += str[i];
        ++i;
      }
      --i;
      postfix += deliminator;
    } else if (is_operator(c)) {
      while (!stack.empty() && !has_lower_precedence(stack.top(), c)) {
        postfix += stack.pop();
      }
      stack.push(c);
    } else if (c == '(') {
      stack.push(c);
    } else if (c == ')') {
      while (!stack.empty() && stack.top() != '(') {
        postfix += stack.pop();
      }
      stack.pop();
    }
  }
  while (!stack.empty()) {
    postfix += stack.pop();
  }
  return postfix;
}

std::string convert_infix_to_prefix(std::string_view str,
                                    char deliminator = ' ') {}

int evaluate_prefix(std::string_view str, char deliminator = ' ') {}
} // namespace

namespace algorithms {

bool check_balanced_parenthesis(std::string_view str) {
  Stack<char> stack{};
  for (char c : str) {
    if (is_open_parenthesis(c)) {
      stack.push(c);
    } else if (is_close_parenthesis(c)) {
      bool matched{stack.size() > 0 && are_parentheses_matched(stack.pop(), c)};
      if (!matched) {
        return false;
      }
    }
  }
  return stack.size() == 0;
};

int evaluate_arithmetic_expression(std::string_view str, char deliminator) {
  return evaluate_postfix(convert_infix_to_postfix(str, deliminator));
}

} // namespace algorithms