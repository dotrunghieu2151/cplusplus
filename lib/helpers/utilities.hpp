#pragma once
#include <functional>
#include <numeric>
#include <cstddef>
#include <array>
#include <vector>
#include <iterator>
#include <algorithm>
#include <concepts>

namespace utilities
{

  template <typename T>
  constexpr void swap(T &a, T &b)
  {
    T temp{a};
    a = b;
    b = temp;
  }

  namespace array
  {
    template <typename T, int N>
    constexpr void selection_sort(T (&arr)[N])
    {
      for (int i{0}; i < N; ++i)
      {
        int minIndex{i};
        for (int j{minIndex + 1}; j < N; ++j)
        {
          if (arr[minIndex] > arr[j])
          {
            minIndex = j;
          }
        }
        if (minIndex != i)
        {
          swap(arr[i], arr[minIndex]);
        }
      }
    }

    template <typename T, int N>
    constexpr void bubble_sort(T (&arr)[N])
    {
      int hasSwapped{false};
      int unsortedElements{N};
      for (; unsortedElements > 0; --unsortedElements)
      {
        for (int index{0}; index < unsortedElements - 1; ++index)
        {
          if (arr[index] > arr[index + 1])
          {
            swap(arr[index], arr[index + 1]);
            hasSwapped = true;
          }
        }
        if (!hasSwapped)
        {
          return;
        }
      }
    }

    template <typename T, std::size_t N>
    constexpr void bubble_sort(std::array<T, N> &arr)
    {
      int hasSwapped{false};
      int unsortedElements{N};
      for (; unsortedElements > 0; --unsortedElements)
      {
        for (int index{0}; index < unsortedElements - 1; ++index)
        {
          if (arr.at(index) > arr.at(index + 1))
          {
            swap(arr.at(index), arr.at(index + 1));
            hasSwapped = true;
          }
        }
        if (!hasSwapped)
        {
          return;
        }
      }
    }

    template <typename T>
    constexpr void bubble_sort(std::vector<T> &arr)
    {
      int hasSwapped{false};
      int unsortedElements{static_cast<int>(arr.size())};
      for (; unsortedElements > 0; --unsortedElements)
      {
        for (int index{0}; index < unsortedElements - 1; ++index)
        {
          if (arr.at(index) > arr.at(index + 1))
          {
            swap(arr.at(index), arr.at(index + 1));
            hasSwapped = true;
          }
        }
        if (!hasSwapped)
        {
          return;
        }
      }
    }

    template <typename Iterator, std::invocable<Iterator, Iterator> F = bool (*)(Iterator, Iterator)>
    constexpr void bubble_sort(
        const Iterator start, const Iterator end, F compareFn = [](Iterator a, Iterator b) -> bool
        { return *a > *b; })
    {
      int hasSwapped{false};
      int unsortedElements{static_cast<int>(std::distance(start, end))};
      for (; unsortedElements > 0; --unsortedElements)
      {
        auto pointer{start};
        for (int step{0}; step < unsortedElements - 1; ++step, ++pointer)
        {
          auto pointerNext{pointer};
          if (compareFn(pointer, ++pointerNext))
          {
            swap(*pointer, *pointerNext);
            hasSwapped = true;
          }
        }
        if (!hasSwapped)
        {
          return;
        }
      }
    }

    template <typename T, std::size_t N>
    constexpr int binary_search(T const (&arr)[N], T target, int min, int max)
    {
      if (min > max)
        return -1;
      int midPoint{std::midpoint(min, max)};
      // int midPoint{min + ((max - min) / 2)};
      if (arr[midPoint] == target)
      {
        return midPoint;
      }
      else if (arr[midPoint] < target)
      {
        return binary_search(arr, target, midPoint + 1, max);
      }
      else
      {
        return binary_search(arr, target, min, midPoint - 1);
      }
    }

    template <typename T, std::size_t N>
    constexpr int binary_search(const std::array<T, N> &arr, T target, int min, int max)
    {
      if (min > max)
        return -1;
      int midPoint{std::midpoint(min, max)};
      // int midPoint{min + ((max - min) / 2)};
      if (arr.at(midPoint) == target)
      {
        return midPoint;
      }
      else if (arr.at(midPoint) < target)
      {
        return binary_search(arr, target, midPoint + 1, max);
      }
      else
      {
        return binary_search(arr, target, min, midPoint - 1);
      }
    }

    template <typename T>
    constexpr int binary_search(const std::vector<T> &arr, T target, int min, int max)
    {
      if (min > max)
        return -1;
      int midPoint{std::midpoint(min, max)};
      // int midPoint{min + ((max - min) / 2)};
      if (arr.at(midPoint) == target)
      {
        return midPoint;
      }
      else if (arr.at(midPoint) < target)
      {
        return binary_search(arr, target, midPoint + 1, max);
      }
      else
      {
        return binary_search(arr, target, min, midPoint - 1);
      }
    }

    template <typename Iterator, typename T>
    constexpr Iterator binary_search(Iterator start, const Iterator end, T target)
    {
      int count{static_cast<int>(std::distance(start, end))};
      while (count > 0)
      {
        auto pointer{start};
        int step{count / 2};
        std::advance(pointer, step);
        if (*pointer == target)
        {
          return pointer;
        }
        else if (*pointer < target)
        {
          start = ++pointer;
          count -= step + 1;
        }
        else
        {
          count = step;
        }
      }

      return end; // if no ele >= target, return end.
    }

    template <typename Iterator, typename T, std::invocable<Iterator, T> F>
    constexpr Iterator binary_search(Iterator start, const Iterator end, T target, F compareFn)
    {
      int count{static_cast<int>(std::distance(start, end))};
      while (count > 0)
      {
        auto pointer{start};
        int step{count / 2};
        std::advance(pointer, step);
        if (compareFn(pointer, target) == 0)
        {
          return pointer;
        }
        else if (compareFn(pointer, target) == -1)
        {
          start = ++pointer;
          count -= step + 1;
        }
        else
        {
          count = step;
        }
      }

      return end; // if no ele >= target, return end.
    }
  };
};