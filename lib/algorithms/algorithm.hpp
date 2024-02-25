#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <functional>
#include <iterator>
#include <stack.hpp>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector.hpp>

// midpoint using Floyd's slow and fast pointer algo
template <std::forward_iterator Iter> Iter midpoint(Iter start, Iter end) {
  Iter slow{start};
  Iter fast{start};

  while (fast != end) {
    ++fast;
    if (fast == end) {
      break;
    }
    ++fast;
    ++slow;
  }

  return slow;
}

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

Vector<Vector<std::pair<int, int>>>
all_paths(Vector<Vector<int>>& maze, int start_x, int start_y, int destination);
} // namespace path_finder

namespace sort {

template <std::random_access_iterator Iter,
          typename Compare = std::less_equal<typename Iter::value_type>>
void bubble_sort(Iter startIter, Iter endIter, Compare compareFn = Compare()) {
  using std::swap;
  const int distance{(int)std::distance(startIter, endIter)};
  bool swapped{false};
  for (int step{}; step < distance - 1; ++step) {
    for (int i{}; i < distance - step - 1; ++i) {
      auto& ele{*(startIter + i)};
      auto& nextEle{*(startIter + i + 1)};
      if (!compareFn(ele, nextEle)) {
        swap(ele, nextEle);
        swapped = true;
      }
    }
    if (!swapped) {
      return;
    }
  }
}

template <std::random_access_iterator Iter,
          typename Compare = std::less_equal<typename Iter::value_type>>
void selection_sort(Iter startIter, Iter endIter,
                    Compare compareFn = Compare()) {
  using std::swap;
  const int distance{(int)std::distance(startIter, endIter)};
  for (int step{}; step < distance - 1; ++step) {
    int min{step};
    for (int i{step + 1}; i < distance; ++i) {
      auto& ele{*(startIter + i)};
      auto& minEle{*(startIter + min)};
      if (!compareFn(minEle, ele)) {
        min = i;
      }
    }
    swap(*(startIter + min), *(startIter + step));
  }
}

template <std::random_access_iterator Iter,
          typename Compare = std::less_equal<typename Iter::value_type>>
void insertion_sort(Iter startIter, Iter endIter,
                    Compare compareFn = Compare()) {
  using std::swap;
  const int distance{(int)std::distance(startIter, endIter)};
  for (int i{1}; i < distance; ++i) {
    for (int j{i};
         j >= 1 && !compareFn(*(startIter + (j - 1)), *(startIter + j));
         j -= 1) {
      swap(*(startIter + (j - 1)), *(startIter + j));
    }
  }
}

namespace {

template <std::forward_iterator Iter,
          typename Compare = std::less_equal<typename Iter::value_type>>
void _merge_sort(Iter startIter, Iter midIter, Iter endIter,
                 Compare compareFn = Compare()) {
  using std::swap;
  // combine as well as base case

  // duplicate 2 subarrays
  Vector<typename Iter::value_type> subArr1(startIter, midIter);
  Vector<typename Iter::value_type> subArr2(midIter, endIter);

  // maintain 3 pointers
  typename Vector<typename Iter::value_type>::Iterator i{subArr1.begin()};
  typename Vector<typename Iter::value_type>::Iterator j{subArr2.begin()};
  Iter k{startIter};

  while (i != subArr1.end() && j != subArr2.end()) {
    if (compareFn(*i, *j)) {
      *k = std::move(*i);
      ++i;
    } else {
      *k = std::move(*j);
      ++j;
    }
    ++k;
  }

  if (i == subArr1.end()) {
    while (j != subArr2.end()) {
      *k = std::move(*j);
      ++j;
      ++k;
    }
  }

  if (j == subArr2.end()) {
    while (i != subArr1.end()) {
      *k = std::move(*i);
      ++i;
      ++k;
    }
  }

  return;
}

template <std::random_access_iterator Iter,
          typename Compare = std::less_equal<typename Iter::value_type>>
Iter _partition(Iter startIter, Iter endIter, Compare compareFn = Compare()) {
  using std::swap;
  // set last index as pivot. We can apply random pivot index by swapping that
  // index with the end -1, and pass in this function
  Iter pivotIter{endIter - 1};

  Iter i{compareFn(*startIter, *pivotIter) ? startIter + 1 : startIter};

  while (++startIter != endIter) {
    if (startIter == pivotIter) {
      continue;
    }

    if (compareFn(*startIter, *pivotIter)) {
      swap(*startIter, *i);
      if (*startIter == *pivotIter) {
        pivotIter = startIter;
      }
      ++i;
    }
  }
  swap(*i, *pivotIter);
  return i;
}
} // namespace

template <std::random_access_iterator Iter,
          typename Compare = std::less_equal<typename Iter::value_type>>
void merge_sort(Iter startIter, Iter endIter, Compare compareFn = Compare()) {
  // end condition
  if (!(startIter != endIter)) {
    return;
  }

  // divide
  int count{(int)std::distance(startIter, endIter)};
  Iter midIter{startIter + count / 2};
  if (count > 1) {
    // conquer
    merge_sort(startIter, midIter, compareFn);
    merge_sort(midIter, endIter, compareFn);
    // combine
    _merge_sort(startIter, midIter, endIter, compareFn);
  }
}

template <std::forward_iterator Iter,
          typename Compare = std::less_equal<typename Iter::value_type>>
void merge_sort(Iter startIter, Iter endIter, Compare compareFn = Compare()) {
  // end condition
  Iter tempStart{startIter};
  if (!(tempStart != endIter)) {
    return;
  }

  if (++tempStart != endIter) {
    // divide
    Iter midIter{midpoint(startIter, endIter)};
    // conquer
    merge_sort(startIter, midIter, compareFn);
    merge_sort(midIter, endIter, compareFn);
    // combine
    _merge_sort(startIter, midIter, endIter, compareFn);
  }
}

template <std::random_access_iterator Iter,
          typename Compare = std::less_equal<typename Iter::value_type>>
void quicksort(Iter startIter, Iter endIter, Compare compareFn = Compare()) {
  // end condition
  if (!(startIter != endIter)) {
    return;
  }

  int count{(int)std::distance(startIter, endIter)};
  if (count <= 1) {
    return;
  }
  // divide
  Iter partitionIter{_partition(startIter, endIter, compareFn)};

  // conquer
  quicksort(startIter, partitionIter, compareFn);
  quicksort(partitionIter + 1, endIter, compareFn);
}

// sort without any comparison
// for small size int (no negative int)
// Time: O(n + k)
// Space: O(max)
void counting_sort(Vector<int>& arr);

void radix_sort(Vector<int>& arr);

template <std::random_access_iterator Iter>
  requires std::integral<typename Iter::value_type> ||
           std::floating_point<typename Iter::value_type>
void bucket_sort(Iter start, Iter end) {
  // dividing elements into buckets
  // just a simlpe algo for putting items into bucket
  // set 10 bucket, and add elements based on their value
  constexpr int bucketCount{10};
  typename Iter::value_type max{};
  for (Iter i{start}; i != end; ++i) {
    max = std::max(max, *i);
  }
  double interval = max / 10.0;

  std::array<Vector<typename Iter::value_type>, bucketCount> buckets{};

  // put items into bucket
  for (Iter i{start}; i != end; ++i) {
    double quotient = (*i) / interval;
    typename Iter::value_type dividend = (int)quotient * interval;
    std::size_t index = dividend == *i && quotient > 0
                            ? (std::size_t)(quotient)-1
                            : (std::size_t)quotient;
    buckets[index].push_back(*i);
  }

  // sort individual bucket
  for (std::size_t i{}; i < buckets.size(); ++i) {
    quicksort(buckets[i].begin(), buckets[i].end(),
              std::less_equal<typename Iter::value_type>());
  }

  // gather buckets
  Iter k{start};
  for (std::size_t i{}; i < buckets.size(); ++i) {
    for (auto j : buckets[i]) {
      *start = j;
      ++start;
    }
  }
  return;
}

template <std::random_access_iterator Iter,
          typename CompareFn = std::less_equal<typename Iter::value_type>>
void heapify(Iter start, Iter root, int size,
             CompareFn compareFn = CompareFn()) {
  using std::swap;
  int parentIndex = std::distance(start, root);

  int leftChildIndex{parentIndex * 2 + 1};
  int rightChildIndex{leftChildIndex + 1};

  int nextIndex{leftChildIndex};

  if (leftChildIndex >= size) {
    return;
  }

  if (rightChildIndex < size &&
      compareFn(*(start + rightChildIndex), *(start + nextIndex))) {
    nextIndex = rightChildIndex;
  }

  if (compareFn(*(start + nextIndex), *(start + parentIndex))) {
    swap(*(start + nextIndex), *(start + parentIndex));
    return heapify(start, start + nextIndex, size, compareFn);
  }
}

template <std::random_access_iterator Iter,
          typename CompareFn = std::less_equal<typename Iter::value_type>>
void heap_sort(Iter start, Iter end, CompareFn compareFn = CompareFn()) {
  using std::swap;
  int distance = std::distance(start, end);
  int lastNoneLeafNode{distance / 2 - 1};
  std::function<bool(const typename Iter::value_type&,
                     const typename Iter::value_type&)>
      reverseCompare{[&compareFn](const typename Iter::value_type& v1,
                                  const typename Iter::value_type& v2) {
        return !compareFn(v1, v2);
      }};
  // heapify entire array
  for (int i{lastNoneLeafNode}; i >= 0; --i) {
    heapify(start, start + i, distance, reverseCompare);
  }

  // heapsort
  for (int i{distance - 1}; i >= 0; --i) {
    swap(*start, *(start + i));
    heapify(start, start, i, reverseCompare);
  }

  return;
}

template <std::random_access_iterator Iter,
          typename Compare = std::less_equal<typename Iter::value_type>>
void shell_sort(Iter startIter, Iter endIter, Compare compareFn = Compare()) {
  using std::swap;
  int distance = std::distance(startIter, endIter);

  // sort for each interval N/2, N/4, N/8 .... until interval = 0
  for (int interval{distance / 2}; interval > 0; interval /= 2) {
    for (int i{interval}; i < distance; ++i) {
      for (int j{i}; j >= interval && !compareFn(*(startIter + (j - interval)),
                                                 *(startIter + j));
           j -= interval) {
        swap(*(startIter + (j - interval)), *(startIter + j));
      }
    }
  }
}

} // namespace sort

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