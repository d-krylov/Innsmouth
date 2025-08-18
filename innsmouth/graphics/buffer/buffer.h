#ifndef INNSMOUTH_BUFFER_H
#define INNSMOUTH_BUFFER_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"

namespace Innsmouth {

class Buffer {
public:
  Buffer();

  ~Buffer();

private:
  VkBuffer buffer_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_BUFFER_H