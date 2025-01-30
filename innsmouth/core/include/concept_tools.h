#ifndef INNSMOUTH_CONCEPT_TOOLS_H
#define INNSMOUTH_CONCEPT_TOOLS_H

#include <concepts>
#include <utility>

namespace Innsmouth {

template <typename T>
concept ScopedEnum = std::is_scoped_enum_v<T>;

template <typename T>
concept Enum = std::is_enum_v<T>;

template <ScopedEnum T> struct is_allowed_enum : std::false_type {};

#define ALLOW_BITMASK_ENUM(EnumType)                                                                                                       \
  template <> struct is_allowed_enum<EnumType> : std::true_type {};

template <typename T>
concept AllowedBitmaskEnum = is_allowed_enum<T>::value;

template <AllowedBitmaskEnum E> E inline constexpr operator|(E LHS, E RHS) {
  return static_cast<E>(std::to_underlying(LHS) | std::to_underlying(RHS));
}

template <AllowedBitmaskEnum E> E inline constexpr operator&(E LHS, E RHS) {
  return static_cast<E>(std::to_underlying(LHS) & std::to_underlying(RHS));
}

template <ScopedEnum E, Enum U> inline constexpr bool operator==(E LHS, U RHS) {
  return std::to_underlying(LHS) == std::to_underlying(RHS);
}

template <AllowedBitmaskEnum Enum> bool HasBits(Enum LHS, Enum RHS) { return (LHS & RHS) == RHS; }

} // namespace Innsmouth

#endif // INNSMOUTH_CONCEPT_TOOLS_H