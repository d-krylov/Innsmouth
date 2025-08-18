#ifndef INNSMOUTH_TYPE_TOOLS_H
#define INNSMOUTH_TYPE_TOOLS_H

#include <tuple>

namespace Innsmouth {

template <typename T> struct FunctionTraits;

template <typename R, typename... A> struct FunctionTraits<R (*)(A...)> {
  using arguments_t = std::tuple<A...>;
};

template <typename R, typename... A> struct FunctionTraits<R(A...)> {
  using arguments_t = std::tuple<A...>;
};

} // namespace Innsmouth

#endif // INNSMOUTH_TYPE_TOOLS_H