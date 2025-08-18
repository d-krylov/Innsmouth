#ifndef INNSMOUTH_SEMAPHORE_H
#define INNSMOUTH_SEMAPHORE_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"

namespace Innsmouth {

class Semaphore {
public:
  Semaphore();

  ~Semaphore();

  operator const VkSemaphore &() const {
    return semaphore_;
  }

  const VkSemaphore *get() const {
    return &semaphore_;
  }

private:
  VkSemaphore semaphore_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_SEMAPHORE_H