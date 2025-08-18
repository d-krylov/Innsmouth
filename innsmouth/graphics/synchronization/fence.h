#ifndef INNSMOUTH_FENCE_H
#define INNSMOUTH_FENCE_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"

namespace Innsmouth {

class Fence {
public:
  Fence(bool signaled);

  ~Fence();

  Fence(Fence &&) noexcept;

  void Wait();
  void Reset();

  operator const VkFence &() const {
    return fence_;
  }

private:
  VkFence fence_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_FENCE_H
