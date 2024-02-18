#include <algorithm.hpp>
#include <algorithm>
#include <array>
#include <functional>
#include <queue.hpp>
#include <stack.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>

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
  std::size_t pointerIndex{static_cast<std::size_t>(
      std::find(std::begin(closeParenthesis), std::end(closeParenthesis),
                closingParenthesis) -
      std::begin(closeParenthesis))};
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
  for (std::size_t i{}; i < str.length(); ++i) {
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
  for (std::size_t i{}; i < str.length(); ++i) {
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

void _premute(std::string& in, std::string& out, Vector<std::string>& result) {
  using std::swap;

  if (in.size() == out.size()) {
    result.push_back(out);
    return;
  }

  std::size_t start{out.size()};

  for (std::size_t i{start}; i < in.size(); ++i) {
    // constraints + action
    out.push_back(in[i]);

    swap(in[start], in[i]);

    // recursion
    _premute(in, out, result);

    // backtrack
    swap(in[i], in[start]);

    out.pop_back();
  }
  return;
}

void _premute_distinct(std::string& in, std::string& out,
                       Vector<std::string>& result) {
  using std::swap;

  if (in.size() == out.size()) {
    result.push_back(out);
    return;
  }

  std::size_t start{out.size()};

  std::unordered_set<char> set{};

  for (std::size_t i{start}; i < in.size(); ++i) {
    if (set.find(in[i]) != set.end()) {
      continue;
    } else {
      set.insert(in[i]);
    }
    // constraints + action
    out.push_back(in[i]);

    swap(in[start], in[i]);

    // recursion
    _premute_distinct(in, out, result);

    // backtrack
    swap(in[i], in[start]);

    out.pop_back();
  }
  return;
}

// std::string convert_infix_to_prefix(std::string_view str,
//                                     char deliminator = ' ') {}

// int evaluate_prefix(std::string_view str, char deliminator = ' ') {}
} // namespace

namespace algorithms {
using namespace std::string_literals;

Vector<std::string> premute(std::string_view str) {
  std::string temp{str};
  std::string out{""s};
  out.reserve(str.size());
  Vector<std::string> result{};

  _premute(temp, out, result);
  return result;
};

Vector<std::string> premute_distinct(std::string_view str) {
  std::string temp{str};
  std::string out{""s};
  out.reserve(str.size());
  Vector<std::string> result{};

  _premute_distinct(temp, out, result);
  return result;
};

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

namespace nqueen {

namespace {
class Board {

public:
  using Piece = std::pair<std::size_t, std::size_t>;
  using PieceSet = std::unordered_set<
      Piece, std::function<std::size_t(const Piece&)>,
      std::function<std::size_t(const Piece&, const Piece&)>>;

  Vector<Vector<std::size_t>> _board{};

  PieceSet _queens{10, [](const Piece& x) { return x.first ^ x.second; },
                   [](const Piece& p1, const Piece& p2) {
                     return (p1.first == p2.first && p1.second == p2.second);
                   }};

private:
  bool is_attacking(const Piece& p1, const Piece& p2) {
    // if 2 pieces are on the same row OR column
    if (p1.first == p2.first || p1.second == p2.second) {
      return true;
    }

    // if 2 pieces are diagonally aligned
    if (std::abs(static_cast<int>(p1.first) - static_cast<int>(p2.first)) ==
        std::abs(static_cast<int>(p1.second) - static_cast<int>(p2.second))) {
      return true;
    }

    return false;
  }

public:
  Board(std::size_t row, std::size_t column)
      : _board(row, Vector<std::size_t>(column, 0)) {}

  void place_queen(std::size_t row, std::size_t column) {
    _board[row][column] = 1;
    _queens.insert({row, column});
  }

  void remove_queen(std::size_t row, std::size_t column) {
    _board[row][column] = 0;
    _queens.erase({row, column});
  }

  bool is_safe(std::size_t row, std::size_t column) {
    for (const std::pair<std::size_t, std::size_t>& pair : _queens) {
      if (row == pair.first && column == pair.second) {
        return false;
      }

      if (is_attacking({row, column}, pair)) {
        return false;
      }
    }
    return true;
  }

  void reset() {
    for (auto& pair : _queens) {
      _board[pair.first][pair.second] = 0;
    }
    _queens.clear();
  }
};

// tries to put queen in a row
// if this returns true meaning we successfully put in a queen, then we can skip
// this row, and move onto the next row
bool solve(Board& board, int queenNum, std::size_t row,
           std::size_t column = 0) {
  if (queenNum == 0) {
    return true;
  }

  for (std::size_t i{row}; i < board._board.size(); ++i) {
    for (std::size_t j{column}; j < board._board[i].size(); ++j) {
      // try to place queen
      if (board.is_safe(i, j)) {
        board.place_queen(i, j);
        if (solve(board, queenNum - 1, i + 1)) {
          return true;
        }
      }

      board.remove_queen(i, j);
    }
    column = 0;
  }
  return false;
}

bool solve_all(Board& board, int queenNum, std::size_t row, std::size_t column,
               Vector<Vector<std::pair<std::size_t, std::size_t>>>& result) {
  if (queenNum == 0) {
    return true;
  }

  for (std::size_t i{row}; i < board._board.size(); ++i) {
    for (std::size_t j{column}; j < board._board[i].size(); ++j) {
      // try to place queen
      if (board.is_safe(i, j)) {
        board.place_queen(i, j);
        if (solve_all(board, queenNum - 1, i + 1, 0, result)) {
          Vector<std::pair<std::size_t, std::size_t>> solution(
              board._queens.size());
          for (auto& pair : board._queens) {
            solution.push_back(pair);
          }
          result.push_back(solution);
        }
      }

      board.remove_queen(i, j);
    }
    column = 0;
  }
  return false;
}

} // namespace

Vector<std::pair<std::size_t, std::size_t>>
n_queen(std::size_t row, std::size_t column, int queenNum) {
  Board board{row, column};
  if (!solve(board, queenNum, 0)) {
    return {};
  }
  Vector<std::pair<std::size_t, std::size_t>> result(board._queens.size());
  for (auto& pair : board._queens) {
    result.push_back(pair);
  }
  return result;
}

Vector<Vector<std::pair<std::size_t, std::size_t>>>
n_queen_all(std::size_t row, std::size_t column, int queenNum) {
  Vector<Vector<std::pair<std::size_t, std::size_t>>> result{};
  Board board{row, column};
  solve_all(board, queenNum, 0, 0, result);
  return result;
}

} // namespace nqueen

namespace knight_tour {
namespace {
class Board {
public:
  Vector<Vector<int>> _board{};
  int lastVisitedCellIndex{-1};
  int totalCells{};

  static constexpr std::array<int, 8> knight_x_dist{2, 1, -1, -2, -2, -1, 1, 2};
  static constexpr std::array<int, 8> knight_y_dist{1, 2, 2, 1, -1, -2, -2, -1};

  Board(int row, int column)
      : _board((std::size_t)row, Vector((std::size_t)column, -1)),
        totalCells{row * column} {}

  bool can_move(int x_knight, int y_knight) {
    return (x_knight >= 0 && x_knight < (int)_board[0].size() &&
            y_knight >= 0 && y_knight < (int)_board.size() &&
            _board[(std::size_t)y_knight][(std::size_t)x_knight] == -1);
  }

  void move_knight(int x_knight, int y_knight) {
    _board[(std::size_t)y_knight][(std::size_t)x_knight] =
        ++lastVisitedCellIndex;
  }

  void remove_knight(int x_knight, int y_knight) {
    _board[(std::size_t)y_knight][(std::size_t)x_knight] = -1;
    --lastVisitedCellIndex;
  }

  int get_degree(int x_knight, int y_knight) {
    int count{};
    for (int i{}; i < (int)knight_x_dist.size(); ++i) {
      int new_x{x_knight + knight_x_dist[(std::size_t)i]};
      int new_y{y_knight + knight_y_dist[(std::size_t)i]};
      if (can_move(new_x, new_y)) {
        ++count;
      }
    }
    return count;
  }

  bool can_end_tour(int x_knight, int y_knight, int start_x, int start_y) {
    for (int i{}; i < (int)knight_x_dist.size(); ++i) {
      int new_x{x_knight + knight_x_dist[(std::size_t)i]};
      int new_y{y_knight + knight_y_dist[(std::size_t)i]};
      if (new_x == start_x && new_y == start_y) {
        return true;
      }
    }
    return false;
  }

  int get_next_move(int x_knight, int y_knight,
                    const std::unordered_set<int>& exclude) {
    int min_degree{knight_x_dist.size() + 1};
    int min_index{-1};

    for (int i{}; i < (int)knight_x_dist.size(); ++i) {
      if (exclude.find(i) != exclude.end()) {
        continue;
      }
      int new_x{x_knight + knight_x_dist[(std::size_t)i]};
      int new_y{y_knight + knight_y_dist[(std::size_t)i]};
      int degree{get_degree(new_x, new_y)};
      if (can_move(new_x, new_y) && degree < min_degree) {
        min_degree = degree;
        min_index = i;
      }
    }

    return min_index;
  }
};

bool solve_naive(Board& board, int x_knight, int y_knight) {
  // all cells visited, return true
  if (board.lastVisitedCellIndex == board.totalCells - 1) {
    return true;
  }

  for (int i{}; i < (int)board.knight_y_dist.size(); ++i) {
    int nextX{x_knight + board.knight_x_dist[(std::size_t)i]};
    int nextY{y_knight + board.knight_y_dist[(std::size_t)i]};

    if (board.can_move(nextX, nextY)) {
      board.move_knight(nextX, nextY);

      if (solve_naive(board, nextX, nextY)) {
        return true;
      }

      board.remove_knight(nextX, nextY);
    }
  }

  return false;
}

bool solve_with_warnsdorff(Board& board, int x_knight, int y_knight) {
  // all cells visited, return true
  if (board.lastVisitedCellIndex == board.totalCells - 1) {
    return true;
  }
  std::unordered_set<int> exclude{};
  for (int i{}; i < (int)board.knight_y_dist.size(); ++i) {
    int nextIndex{board.get_next_move(x_knight, y_knight, exclude)};

    if (nextIndex > -1) {
      int nextX{x_knight + board.knight_x_dist[(std::size_t)nextIndex]};
      int nextY{y_knight + board.knight_y_dist[(std::size_t)nextIndex]};
      board.move_knight(nextX, nextY);

      if (solve_with_warnsdorff(board, nextX, nextY)) {
        return true;
      }

      board.remove_knight(nextX, nextY);
    } else {
      exclude.insert(i);
    }
  }

  return false;
}
} // namespace

Vector<Vector<int>> solve_knight_tour_naive(int row, int column) {
  Board board{row, column};
  board.move_knight(0, 0);
  if (!solve_naive(board, 0, 0)) {
    return {};
  }

  return board._board;
}

Vector<Vector<int>> solve_knight_tour_warnsdorff(int row, int column) {
  Board board{row, column};
  board.move_knight(0, 0);
  if (!solve_with_warnsdorff(board, 0, 0)) {
    return {};
  }

  return board._board;
}

} // namespace knight_tour

namespace path_finder {
using Maze = Vector<Vector<int>>;
namespace {
bool can_move(Maze& maze, int x, int y, int maxX, int maxY, int dest) {
  return x >= 0 && x < maxX && y >= 0 && y < maxY &&
         (maze[(std::size_t)y][(std::size_t)x] ||
          maze[(std::size_t)y][(std::size_t)x] == dest);
}
} // namespace

Vector<std::pair<int, int>> shortest_path(Maze& maze, int start_x, int start_y,
                                          int destination) {
  std::array<int, 4> dx{1, 0, 0, -1};
  std::array<int, 4> dy{0, 1, -1, 0};

  bool pathExists{false};
  Queue<int> queue_x{};
  Queue<int> queue_y{};
  Vector<std::pair<int, int>> result{};
  std::unordered_set<int*> visited{};
  std::unordered_map<int*, std::pair<int, int>> prev{};

  queue_x.push_back(start_x);
  queue_y.push_back(start_y);
  visited.insert(&maze[(std::size_t)start_y][(std::size_t)start_x]);

  while (queue_x.size()) {
    int x{queue_x.pop_front()};
    int y{queue_y.pop_front()};

    for (int i{}; i < (int)dx.size(); ++i) {
      int nextX{x + dx[(std::size_t)i]};
      int nextY{y + dy[(std::size_t)i]};

      if (can_move(maze, nextX, nextY, maze[0].size(), maze.size(),
                   destination) &&
          visited.find(&maze[(std::size_t)nextY][(std::size_t)nextX]) ==
              visited.end()) {
        prev.insert_or_assign(&maze[(std::size_t)nextY][(std::size_t)nextX],
                              std::make_pair(x, y));
        if (maze[(std::size_t)nextY][(std::size_t)nextX] == destination) {
          pathExists = true;
          result.push_back({nextX, nextY});
          break;
        }
        queue_x.push_back(nextX);
        queue_y.push_back(nextY);
        visited.insert(&maze[(std::size_t)nextY][(std::size_t)nextX]);
      }
    }
  }

  if (!pathExists) {
    return result;
  }

  // reconstruct path
  std::pair<int, int> end{result.back()};
  int* endPtr{&maze[(std::size_t)end.second][(std::size_t)end.first]};
  for (; endPtr != nullptr;
       endPtr = &maze[(std::size_t)end.second][(std::size_t)end.first]) {
    auto next{prev.find(endPtr)};
    if (next == prev.end()) {
      break;
    }
    end = next->second;
    result.push_back(next->second);
  }

  // reverse path
  result.reverse();
  return result;
};

Vector<Vector<std::pair<int, int>>> all_paths(Maze& maze, int start_x,
                                              int start_y, int destination) {
  std::array<int, 4> dx{1, 0, 0, -1};
  std::array<int, 4> dy{0, 1, -1, 0};

  Vector<Vector<std::pair<int, int>>> result{};

  for (std::size_t pathIndex{}; pathIndex < dx.size(); ++pathIndex) {
    Stack<int> stack_x{};
    Stack<int> stack_y{};
    Vector<std::pair<int, int>> path{};
    std::unordered_set<int*> visited{};
    std::unordered_map<int*, int*> prev{};
    path.push_back({start_x, start_y});
    visited.insert(&maze[(std::size_t)start_y][(std::size_t)start_x]);

    int new_x_start{start_x + dx[pathIndex]};
    int new_y_start{start_y + dy[pathIndex]};

    if (can_move(maze, new_x_start, new_y_start, maze[0].size(), maze.size(),
                 destination)) {
      bool pathExists{false};
      stack_x.push(new_x_start);
      stack_y.push(new_y_start);

      while (stack_x.size()) {
        pathExists = false;
        int x{stack_x.pop()};
        int y{stack_y.pop()};

        int* cellPtr{&maze[(std::size_t)y][(std::size_t)x]};

        if (visited.find(cellPtr) == visited.end()) {
          visited.insert(cellPtr);
          path.push_back({x, y});
        }

        for (std::size_t i{}; i < dx.size(); ++i) {
          int new_x{x + dx[i]};
          int new_y{y + dy[i]};

          if (can_move(maze, new_x, new_y, maze[0].size(), maze.size(),
                       destination) &&
              visited.find(&maze[(std::size_t)new_y][(std::size_t)new_x]) ==
                  visited.end()) {
            if (maze[(std::size_t)new_y][(std::size_t)new_x] == destination) {
              path.push_back({new_x, new_y});
              result.push_back(path);
              path.pop_back();
              continue;
            }

            stack_x.push(new_x);
            stack_y.push(new_y);
            prev.insert_or_assign(&maze[(std::size_t)new_y][(std::size_t)new_x],
                                  &maze[(std::size_t)y][(std::size_t)x]);
            pathExists = true;
          }
        }
        if (!pathExists && stack_y.size()) {
          while (prev.at(&maze[(std::size_t)path.back().second]
                              [(std::size_t)path.back().first]) !=
                 prev.at(&maze[(std::size_t)stack_y.top()]
                              [(std::size_t)stack_x.top()])) {
            path.pop_back();
          }
          path.pop_back();
        }
      }
    }
  }

  return result;
};

}; // namespace path_finder
} // namespace algorithms