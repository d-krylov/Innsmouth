#ifndef INNSMOUTH_FENCE_H
#define INNSMOUTH_FENCE_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"

namespace Innsmouth {

class Fence {
public:
  Fence(FenceCreateMask fence_create_mask);

  ~Fence();

  Fence(Fence &&) noexcept;

  void Wait();

  void Reset();

  const VkFence GetHandle() const;

private:
  VkFence fence_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_FENCE_H
