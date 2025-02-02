#ifndef INNSMOUTH_FENCE_H
#define INNSMOUTH_FENCE_H

#include "graphics/include/graphics_types.h"

namespace Innsmouth {

class Fence {
public:
  Fence(bool signaled);

  ~Fence();

  Fence(Fence &&) noexcept;

  void Wait();
  void Reset();

  operator const VkFence &() const { return fence_; }

private:
  VkFence fence_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_FENCE_H
