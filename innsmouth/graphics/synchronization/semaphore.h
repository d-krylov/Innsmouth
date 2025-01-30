#ifndef INNSMOUTH_SEMAPHORE_H
#define INNSMOUTH_SEMAPHORE_H

#include "graphics/include/graphics_types.h"

namespace Innsmouth {

class Semaphore {
public:
  Semaphore();

  ~Semaphore();

private:
  VkSemaphore semaphore_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_SEMAPHORE_H