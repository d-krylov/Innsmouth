#ifndef INNSMOUTH_CONCEPTS_H
#define INNSMOUTH_CONCEPTS_H

#include <ranges>
#include <type_traits>

namespace Innsmouth {

template <typename T> struct is_allowed_enum : std::false_type {};

#define ALLOW_BITMASK_ENUM(EnumType)                                                               \
  template <> struct is_allowed_enum<EnumType> : std::true_type {};

template <typename T> concept AllowedBitmaskEnum = is_allowed_enum<T>::value;

template <AllowedBitmaskEnum Enum> Enum operator|(Enum LHS, Enum RHS) {
  using underlying = std::underlying_type_t<Enum>;
  return static_cast<Enum>(static_cast<underlying>(LHS) | static_cast<underlying>(RHS));
}

} // namespace Innsmouth

#endif // INNSMOUTH_CONCEPTS_H