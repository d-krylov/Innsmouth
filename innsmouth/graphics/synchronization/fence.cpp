#include "fence.h"
#include <utility>

namespace Innsmouth {

Fence::Fence(bool signaled) {
  VkFenceCreateInfo fence_ci{};
  {
    fence_ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_ci.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
  }
  VK_CHECK(vkCreateFence(GraphicsContext::Get().GetDevice(), &fence_ci, nullptr, &fence_));
}

Fence::Fence(Fence &&other) noexcept {
  fence_ = std::exchange(other.fence_, nullptr);
}

Fence::~Fence() {
  // vkDestroyFence(Device(), fence_, nullptr);
}

void Fence::Wait() {
  VK_CHECK(vkWaitForFences(GraphicsContext::Get().GetDevice(), 1, &fence_, VK_TRUE, UINT64_MAX));
}

void Fence::Reset() {
  VK_CHECK(vkResetFences(GraphicsContext::Get().GetDevice(), 1, &fence_));
}

} // namespace Innsmouth