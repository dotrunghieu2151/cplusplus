#include <algorithm.hpp>
#include <array>
#include <gtest/gtest.h>
#include <iostream>
#include <math.hpp>
#include <string>
#include <string_view>
#include <timer.hpp>
#include <vector.hpp>

using namespace std::string_literals;

TEST(ConvertInfixToPostFix, test) {
  EXPECT_EQ(algorithms::evaluate_arithmetic_expression(
                "22*3+(81/3^2^2 + 145-(243+42))"),
            -73);
}

TEST(PrimeNumber, SegementedFindAllPrimeLessThanN) {
  Vector<int> correctResults{2,  3,  5,  7,  11, 13, 17, 19, 23, 29, 31, 37, 41,
                             43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
  Vector<int> results{math::find_prime_segmented(100)};

  EXPECT_EQ(correctResults.size(), results.size());
  for (std::size_t index{}; index < correctResults.size(); ++index) {
    EXPECT_EQ(correctResults[index], results[index]);
    EXPECT_EQ(math::is_prime(results[index]), true);
  }
}

TEST(PrimeNumber, SimpleFindAllPrimeLessThanN) {
  Vector<int> correctResults{2,  3,  5,  7,  11, 13, 17, 19, 23, 29, 31, 37, 41,
                             43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
  Vector<int> results{math::find_prime(100)};

  EXPECT_EQ(correctResults.size(), results.size());
  for (std::size_t index{}; index < correctResults.size(); ++index) {
    EXPECT_EQ(correctResults[index], results[index]);
    EXPECT_EQ(math::is_prime(results[index]), true);
  }
}

TEST(PrimeNumber, FindAllPrimePerfTest) {
  Timer timer{};

  Vector<int> results2{math::find_prime_segmented(40000000)};
  double segmentedPrimeTime{timer.elapsed()};

  std::cout << "Segmented Sieve time: " << segmentedPrimeTime << "\n";

  timer.reset();

  Vector<int> results1{math::find_prime(40000000)};
  double simplePrimeTime{timer.elapsed()};

  std::cout << "Simple Sieve time: " << simplePrimeTime << "\n";

  EXPECT_EQ(results2.size(), results1.size());
  // EXPECT_EQ(simplePrimeTime, segmentedPrimeTime);
}

TEST(Premutation, StringBacktracking) {
  std::string str{"ABC"s};
  Vector<std::string> result{algorithms::premute(str)};
  std::cout << result << "\n";
}

TEST(Premutation, StringDistinctBacktracking) {
  std::string str{"ACBC"s};
  Vector<std::string> result{algorithms::premute_distinct(str)};
  std::cout << result << "\n";
}

TEST(NQueen, OneSolution) {
  Vector<std::pair<std::size_t, std::size_t>> result{
      algorithms::nqueen::n_queen(8, 8, 8)};
  for (auto& pair : result) {
    std::cout << "(" << pair.first << "," << pair.second << ")"
              << "\n";
  }
}

TEST(NQueen, AllSolution) {
  Vector<Vector<std::pair<std::size_t, std::size_t>>> result{
      algorithms::nqueen::n_queen_all(8, 8, 8)};
  EXPECT_EQ(result.size(), 92);
  for (auto& solution : result) {
    std::cout << "[";
    std::cout << "(" << solution[0].first << "," << solution[0].second << ")";
    for (std::size_t i{1}; i < solution.size(); ++i) {
      std::cout << ", (" << solution[i].first << "," << solution[i].second
                << ")";
    }
    std::cout << "]\n";
  }
}

TEST(KnightTour, NaiveBacktracking) {
  Timer timer{};
  Vector<Vector<int>> result{
      algorithms::knight_tour::solve_knight_tour_naive(8, 8)};
  std::cout << "Knight tour naive in: " << timer.elapsed() << "\n";
  EXPECT_EQ(result.size(), 8);
  for (auto& row : result) {
    std::cout << "[";
    std::cout << row[0];
    for (int i{1}; i < (int)row.size(); ++i) {
      std::cout << ", " << row[(std::size_t)i];
    }
    std::cout << "]\n";
  }
}

TEST(KnightTour, WarnsdorffBacktracking) {
  Timer timer{};
  Vector<Vector<int>> result{
      algorithms::knight_tour::solve_knight_tour_warnsdorff(8, 8)};
  std::cout << "Knight tour Warnsdorff in: " << timer.elapsed() << "\n";
  EXPECT_EQ(result.size(), 8);
  for (auto& row : result) {
    std::cout << "[";
    std::cout << row[0];
    for (int i{1}; i < (int)row.size(); ++i) {
      std::cout << ", " << row[(std::size_t)i];
    }
    std::cout << "]\n";
  }
}

TEST(Sudoku, BacktrackingEasy) {
  Timer timer{};
  std::array<std::array<int, 9>, 9> grid{{{9, 0, 2, 8, 1, 0, 0, 0, 5},
                                          {8, 1, 5, 3, 0, 4, 0, 2, 6},
                                          {4, 3, 0, 0, 0, 0, 8, 0, 0},
                                          {5, 8, 0, 0, 7, 0, 0, 0, 3},
                                          {0, 2, 4, 9, 0, 1, 6, 0, 0},
                                          {0, 0, 0, 0, 3, 0, 2, 5, 4},
                                          {6, 0, 8, 0, 0, 3, 4, 0, 0},
                                          {0, 4, 0, 0, 8, 0, 1, 0, 0},
                                          {0, 0, 1, 0, 0, 6, 5, 0, 8}}};
  algorithms::sudoku::Sudoku sudokuSolver{};
  bool canSolve{sudokuSolver.solve(grid)};
  EXPECT_EQ(canSolve, true);
  std::cout << "Sudoku solved in: " << timer.elapsed() << "\n";

  for (auto& row : grid) {
    std::cout << "[";
    std::cout << row[0];
    for (int i{1}; i < (int)row.size(); ++i) {
      std::cout << ", " << row[(std::size_t)i];
    }
    std::cout << "]\n";
  }
}

// TEST(Sudoku, BacktrackingHard) {
//   Timer timer{};
//   std::array<std::array<int, 9>, 9> grid{{{7, 0, 0, 0, 0, 3, 0, 0, 0},
//                                           {0, 3, 4, 6, 0, 0, 0, 0, 1},
//                                           {0, 0, 0, 0, 8, 0, 0, 2, 0},
//                                           {0, 9, 0, 0, 0, 0, 0, 0, 0},
//                                           {0, 1, 5, 3, 0, 0, 0, 0, 4},
//                                           {0, 0, 0, 0, 0, 6, 3, 0, 0},
//                                           {6, 0, 0, 0, 0, 0, 0, 0, 7},
//                                           {0, 5, 7, 0, 0, 2, 0, 1, 0},
//                                           {9, 0, 0, 5, 0, 0, 0, 0, 0}}};
//   algorithms::sudoku::Sudoku sudokuSolver{};
//   bool canSolve{sudokuSolver.solve(grid)};
//   EXPECT_EQ(canSolve, true);
//   std::cout << "Sudoku solved in: " << timer.elapsed() << "\n";

//   for (auto& row : grid) {
//     std::cout << "[";
//     std::cout << row[0];
//     for (int i{1}; i < (int)row.size(); ++i) {
//       std::cout << ", " << row[(std::size_t)i];
//     }
//     std::cout << "]\n";
//   }
// }

TEST(PathFinder, ShortestPath) {
  Timer timer{};
  Vector<Vector<int>> maze{
      {{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 2}}};
  Vector<std::pair<int, int>> result{
      algorithms::path_finder::shortest_path(maze, 0, 0, 2)};
  std::cout << "Pathfinder shortest path solved in: " << timer.elapsed()
            << "\n";
  Vector<std::pair<int, int>> paths{
      {{0, 0}, {0, 1}, {1, 1}, {1, 2}, {1, 3}, {2, 3}, {3, 3}}};
  EXPECT_EQ(result.size(), paths.size());
  for (std::size_t i{}; i < paths.size(); ++i) {
    EXPECT_EQ(result[i], paths[i]);
  }
  if (result.size()) {
    std::cout << "(" << result[0].second << "," << result[0].first << ")";
    for (std::size_t i{1}; i < result.size(); ++i) {
      std::cout << " (" << result[i].second << "," << result[i].first << ")";
    }
    std::cout << "\n";
  }
}