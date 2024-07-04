#ifndef INNSMOUTH_ASSERTS_H
#define INNSMOUTH_ASSERTS_H

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <signal.h>

namespace Innsmouth {

#define DEBUG_BREAK() raise(SIGTRAP)

#define CORE_ASSERT_FATAL(format, ...)                                                             \
  do {                                                                                             \
    printf("%s:%u " format "\n", __FILE__, __LINE__, ##__VA_ARGS__);                               \
    abort();                                                                                       \
  } while (1)

#define CORE_VERIFY(expression)                                                                    \
  do {                                                                                             \
    if (!(expression)) {                                                                           \
      CORE_ASSERT_FATAL("assert %s failed in %s", #expression, __func__);                          \
    }                                                                                              \
  } while (0)

#define CORE_UNREACHABLE() __builtin_unreachable()

} // namespace Innsmouth

#endif // INNSMOUTH_ASSERTS_H