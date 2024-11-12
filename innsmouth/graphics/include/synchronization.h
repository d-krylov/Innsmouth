#ifndef INNSMOUTH_SYNCHRONIZATION_H
#define INNSMOUTH_SYNCHRONIZATION_H

#include "graphics/include/graphics_types.h"

namespace Innsmouth {

class Fence {
public:
  Fence(bool signaled);
  ~Fence();

  NO_COPY_SEMANTIC(Fence);

  Fence(Fence &&) noexcept;

  void Wait();
  void Reset();

  operator const VkFence &() const { return fence_; }

private:
  VkFence fence_;
};

class Semaphore {
public:
  Semaphore();

  ~Semaphore();

  Semaphore(Semaphore &&) noexcept;

  NO_COPY_SEMANTIC(Semaphore);

  operator const VkSemaphore &() const { return semaphore_; }

  [[nodiscard]] const VkSemaphore *get() const { return &semaphore_; }

private:
  VkSemaphore semaphore_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_SYNCHRONIZATION_H