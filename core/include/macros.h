#ifndef INNSMOUTH_MACROS_H
#define INNSMOUTH_MACROS_H

namespace Innsmouth {

#define NO_MOVE_CONSTRUCTOR(TypeName) TypeName(TypeName &&) = delete;
#define NO_MOVE_OPERATOR(TypeName) TypeName &operator=(TypeName &&) = delete;

#define NO_MOVE_SEMANTIC(TypeName)                                                                 \
  NO_MOVE_CONSTRUCTOR(TypeName)                                                                    \
  NO_MOVE_OPERATOR(TypeName)

#define NO_COPY_CONSTRUCTOR(TypeName) TypeName(const TypeName &) = delete;
#define NO_COPY_OPERATOR(TypeName) void operator=(const TypeName &) = delete;

#define NO_COPY_SEMANTIC(TypeName)                                                                 \
  NO_COPY_CONSTRUCTOR(TypeName)                                                                    \
  NO_COPY_OPERATOR(TypeName)

} // namespace Innsmouth

#endif // INNSMOUTH_MACROS_H