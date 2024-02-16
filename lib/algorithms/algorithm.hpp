#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <stack.hpp>
#include <string>
#include <string_view>
#include <unordered_set>

namespace algorithms {
bool check_balanced_parenthesis(std::string_view str);

int evaluate_arithmetic_expression(std::string_view str,
                                   char deliminator = ' ');

Vector<std::string> premute(std::string_view str);

Vector<std::string> premute_distinct(std::string_view str);

namespace nqueen {

Vector<std::pair<std::size_t, std::size_t>>
n_queen(std::size_t row, std::size_t column, int queenNum);

Vector<Vector<std::pair<std::size_t, std::size_t>>>
n_queen_all(std::size_t row, std::size_t column, int queenNum);
} // namespace nqueen

namespace knight_tour {

Vector<Vector<int>> solve_knight_tour_naive(int row, int column);

Vector<Vector<int>> solve_knight_tour_warnsdorff(int row, int column);
} // namespace knight_tour

namespace path_finder {
Vector<std::pair<int, int>> shortest_path(Vector<Vector<int>>& maze,
                                          int start_x, int start_y,
                                          int destination);

Vector<Vector<std::pair<int, int>>> all_paths(Vector<Vector<int>>& maze);
} // namespace path_finder

namespace sudoku {

class Sudoku {
public:
  using Grid = std::array<std::array<int, 9>, 9>;

private:
  using Cell = std::pair<int, int>;
  using CellSet =
      std::unordered_set<Cell, std::function<std::size_t(const Cell&)>,
                         std::function<std::size_t(const Cell&, const Cell&)>>;

  CellSet _unassignedCells{10, [](const Cell& x) { return x.first ^ x.second; },
                           [](const Cell& p1, const Cell& p2) {
                             return (p1.first == p2.first &&
                                     p1.second == p2.second);
                           }};

  std::array<int, 9> _row_bitmask{};
  std::array<int, 9> _column_bitmask{};
  std::array<int, 9> _subgrid_bitmask{};

public:
  Sudoku() = default;

  bool solve(Grid& grid) {
    _reset_bitmasks();
    _retrieve_unassiged_cells(grid);

    return _solve(grid);
  };

private:
  void _reset_bitmasks() {
    _row_bitmask = {};
    _column_bitmask = {};
    _subgrid_bitmask = {};
  }

  void _retrieve_unassiged_cells(Grid& grid) {
    for (int i{}; i < 9; ++i) {
      for (int j{}; j < 9; ++j) {
        int number{grid[(std::size_t)i][(std::size_t)j]};
        if (number == 0) {
          _unassignedCells.insert({i, j});
        } else {
          _row_bitmask[(std::size_t)i] |= 1 << number;
          _column_bitmask[(std::size_t)j] |= 1 << number;
          _subgrid_bitmask[(std::size_t)(i / 3 * 3 + j / 3)] |= 1 << number;
        }
      }
    }
  }

  void _assign_number(Grid& grid, int row, int column, int number) {
    grid[(std::size_t)row][(std::size_t)column] = number;
    _row_bitmask[(std::size_t)row] |= 1 << number;
    _column_bitmask[(std::size_t)column] |= 1 << number;
    _subgrid_bitmask[(std::size_t)(row / 3 * 3 + column / 3)] |= 1 << number;
    _unassignedCells.erase({row, column});
  }

  void _unassign_number(Grid& grid, int row, int column) {
    int number{grid[(std::size_t)row][(std::size_t)column]};
    _unassignedCells.insert({row, column});
    _row_bitmask[(std::size_t)row] &= ~(1 << number);
    _column_bitmask[(std::size_t)column] &= ~(1 << number);
    _subgrid_bitmask[(std::size_t)(row / 3 * 3 + column / 3)] &= ~(1 << number);
    grid[(std::size_t)row][(std::size_t)column] = 0;
  }

  bool _solve(Grid& grid) {
    if (_unassignedCells.empty()) {
      return true;
    }

    auto [row, column] = *_unassignedCells.begin();

    for (int i{1}; i <= 9; ++i) {
      if (_is_safe(row, column, i)) {
        _assign_number(grid, row, column, i);
        if (_solve(grid)) {
          return true;
        }

        // backtracking
        _unassign_number(grid, row, column);
      }
    }

    return false;
  }

  bool _is_safe(int row, int column, int number) {
    // check if there is a duplicate in same row
    // check if there is a duplicate in same column
    // check if there is a duplicate in a 3x3 subgrid
    if (!((_row_bitmask[(std::size_t)row] >> number) & 1) &&
        !((_column_bitmask[(std::size_t)column] >> number) & 1) &&
        !((_subgrid_bitmask[(std::size_t)(row / 3 * 3 + column / 3)] >>
           number) &
          1)) {
      return true;
    }

    return false;
  }
};
} // namespace sudoku
} // namespace algorithms