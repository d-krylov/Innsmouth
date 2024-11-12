#include "graphics/include/synchronization.h"
#include "graphics/include/graphics.h"

namespace Innsmouth {

Fence::Fence(bool signaled) {
  VkFenceCreateInfo fence_ci{};
  {
    fence_ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_ci.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
  }
  VK_CHECK(vkCreateFence(Device(), &fence_ci, nullptr, &fence_));
}

Fence::Fence(Fence &&other) noexcept { fence_ = std::exchange(other.fence_, nullptr); }

Fence::~Fence() { vkDestroyFence(Device(), fence_, nullptr); }

void Fence::Wait() { VK_CHECK(vkWaitForFences(Device(), 1, &fence_, VK_TRUE, UINT64_MAX)); }

void Fence::Reset() { VK_CHECK(vkResetFences(Device(), 1, &fence_)); }

Semaphore::Semaphore() {
  VkSemaphoreCreateInfo semaphore_ci{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  VK_CHECK(vkCreateSemaphore(Device(), &semaphore_ci, nullptr, &semaphore_));
}

Semaphore::Semaphore(Semaphore &&other) noexcept { semaphore_ = std::exchange(other.semaphore_, nullptr); }

Semaphore::~Semaphore() { vkDestroySemaphore(Device(), semaphore_, nullptr); }

} // namespace Innsmouth