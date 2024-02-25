#include <algorithm.hpp>
#include <algorithm>
#include <array>
#include <gtest/gtest.h>
#include <iostream>
#include <list.hpp>
#include <math.hpp>
#include <string>
#include <string_view>
#include <timer.hpp>
#include <vector.hpp>

TEST(Sort, BubbleSortMin) {
  Vector<int> arr{4, 6, 3, 4, 5, 2, 0, 1};
  Vector<int> correctArr{0, 1, 2, 3, 4, 4, 5, 6};
  algorithms::sort::bubble_sort(arr.begin(), arr.end());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, BubbleSortMax) {
  Vector<int> arr{4, 6, 3, 3, 5, 2, 0, 1};
  Vector<int> correctArr{6, 5, 4, 3, 3, 2, 1, 0};
  algorithms::sort::bubble_sort(arr.begin(), arr.end(),
                                std::greater_equal<int>());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, SelectionSortMin) {
  Vector<int> arr{4, 6, 3, 4, 5, 2, 0, 1};
  Vector<int> correctArr{0, 1, 2, 3, 4, 4, 5, 6};
  algorithms::sort::selection_sort(arr.begin(), arr.end());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, SelectionSortMax) {
  Vector<int> arr{4, 6, 3, 3, 5, 2, 0, 1};
  Vector<int> correctArr{6, 5, 4, 3, 3, 2, 1, 0};
  algorithms::sort::selection_sort(arr.begin(), arr.end(),
                                   std::greater_equal<int>());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, InsertionSortMin) {
  Vector<int> arr{4, 6, 3, 3, 5, 2, 0, 1};
  Vector<int> correctArr{0, 1, 2, 3, 3, 4, 5, 6};
  algorithms::sort::insertion_sort(arr.begin(), arr.end());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, InsertionSortMax) {
  Vector<int> arr{4, 6, 3, 4, 5, 2, 0, 1};
  Vector<int> correctArr{6, 5, 4, 4, 3, 2, 1, 0};
  algorithms::sort::insertion_sort(arr.begin(), arr.end(),
                                   std::greater_equal<int>());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, MergeSortForArrayMin) {
  Vector<int> arr{6, 4, 4, 3, 5, 2, 0, 1};
  Vector<int> correctArr{0, 1, 2, 3, 4, 4, 5, 6};
  algorithms::sort::merge_sort(arr.begin(), arr.end());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, MergeSortForArrayMax) {
  Vector<int> arr{4, 6, 3, 3, 5, 2, 0, 1};
  Vector<int> correctArr{6, 5, 4, 3, 3, 2, 1, 0};
  algorithms::sort::merge_sort(arr.begin(), arr.end(),
                               std::greater_equal<int>());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, MergeSortForListMin) {
  List<int> list{6, 4, 4, 3, 5, 2, 0, 1};
  Vector<int> correct{0, 1, 2, 3, 4, 4, 5, 6};
  algorithms::sort::merge_sort(list.begin(), list.end());

  List<int>::Iterator startIter{list.begin()};
  for (std::size_t i{}; i < correct.size(); ++i) {
    EXPECT_EQ(*startIter, correct[i]);
    ++startIter;
  }
}

TEST(Sort, MergeSortForListMax) {
  List<int> list{4, 6, 3, 3, 5, 2, 0, 1};
  Vector<int> correct{6, 5, 4, 3, 3, 2, 1, 0};
  algorithms::sort::merge_sort(list.begin(), list.end(),
                               std::greater_equal<int>());

  List<int>::Iterator startIter{list.begin()};
  for (std::size_t i{}; i < correct.size(); ++i) {
    EXPECT_EQ(*startIter, correct[i]);
    ++startIter;
  }
}

TEST(Sort, QuicksortMin) {
  Vector<int> arr{6, 4, 4, 3, 5, 2, 0, 1};
  Vector<int> correctArr{0, 1, 2, 3, 4, 4, 5, 6};
  algorithms::sort::quicksort(arr.begin(), arr.end());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, QuicksortMax) {
  Vector<int> arr{4, 6, 3, 3, 5, 2, 0, 1};
  Vector<int> correctArr{6, 5, 4, 3, 3, 2, 1, 0};
  algorithms::sort::quicksort(arr.begin(), arr.end(),
                              std::greater_equal<int>());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, CountingSortMin) {
  Vector<int> arr{6, 4, 4, 3, 5, 2, 0, 1};
  Vector<int> correctArr{0, 1, 2, 3, 4, 4, 5, 6};
  algorithms::sort::counting_sort(arr);

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, RadixSortMin) {
  Vector<int> arr{6, 4, 4, 3, 5, 2, 0, 1};
  Vector<int> correctArr{0, 1, 2, 3, 4, 4, 5, 6};
  algorithms::sort::radix_sort(arr);

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, BucketSortIntegerMin) {
  Vector<int> arr{6, 4, 4, 3, 5, 2, 0, 1};
  Vector<int> correctArr{0, 1, 2, 3, 4, 4, 5, 6};
  algorithms::sort::bucket_sort(arr.begin(), arr.end());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, BucketSortFloatMin) {
  Vector<float> arr{0.42, 0.32, 0.23, 0.52, 0.25, 0.47, 0.0, 0.51};
  Vector<float> correctArr{0.0, 0.23, 0.25, 0.32, 0.42, 0.47, 0.51, 0.52};
  algorithms::sort::bucket_sort(arr.begin(), arr.end());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, HeapSortMin) {
  Vector<int> arr{6, 4, 4, 3, 5, 2, 0, 1};
  Vector<int> correctArr{0, 1, 2, 3, 4, 4, 5, 6};
  algorithms::sort::heap_sort(arr.begin(), arr.end());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, HeapSortMax) {
  Vector<int> arr{4, 6, 3, 3, 5, 2, 0, 1};
  Vector<int> correctArr{6, 5, 4, 3, 3, 2, 1, 0};
  algorithms::sort::heap_sort(arr.begin(), arr.end(),
                              std::greater_equal<int>());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, ShellSortMin) {
  Vector<int> arr{9, 8, 3, 7, 5, 6, 4, 1};
  Vector<int> correctArr{1, 3, 4, 5, 6, 7, 8, 9};
  algorithms::sort::shell_sort(arr.begin(), arr.end());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}

TEST(Sort, ShellSortMax) {
  Vector<int> arr{4, 6, 3, 3, 5, 2, 0, 1};
  Vector<int> correctArr{6, 5, 4, 3, 3, 2, 1, 0};
  algorithms::sort::shell_sort(arr.begin(), arr.end(),
                               std::greater_equal<int>());

  for (std::size_t i{}; i < correctArr.size(); ++i) {
    EXPECT_EQ(arr[i], correctArr[i]);
  }
}