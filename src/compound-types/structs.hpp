#pragma once

template <typename T, typename U>
struct Fraction
{
  T numerator{};
  U denominator{};
};

template <typename T, typename U>
Fraction(T, U) -> Fraction<T, U>;
