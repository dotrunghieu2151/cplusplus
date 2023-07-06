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

} // namespace concepts