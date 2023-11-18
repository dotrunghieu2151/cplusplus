#pragma once
#include <concepts>

namespace concepts {

template <typename TypeToCheck, typename TypeToCheckAgainst>
concept TypeIs =
    std::same_as<std::remove_cvref_t<TypeToCheck>, TypeToCheckAgainst>;

template <typename TypeToCheck, typename... TypesToCheckAgainst>
concept TypeIn =
    (std::same_as<std::remove_cvref_t<TypeToCheck>, TypesToCheckAgainst> ||
     ...);

template <typename T>
concept IsIterator = requires(T a) { typename T::iterator_category; };

template <typename T>
concept IsStackContainer = requires(T container, const typename T::reference v,
                                    typename T::rvalue_reference vr) {
  typename T::value;
  typename T::reference;
  typename T::rvalue_reference;

  container.push_back(v);
  container.push_back(vr);
  container.pop_back();
  container.size();
  container.back();
  container.empty();
};

template <typename T>
concept Comparable = requires(T ele, T other) {
  ele >= other;
  ele <= other;
  ele > other;
  ele < other;
};

} // namespace concepts