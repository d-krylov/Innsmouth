#include "fence.h"
#include <utility>

namespace Innsmouth {

Fence::Fence(FenceCreateMask fence_create_mask) {
  FenceCreateInfo fence_ci;
  fence_ci.flags = fence_create_mask;
  VK_CHECK(vkCreateFence(GraphicsContext::Get()->GetDevice(), fence_ci, nullptr, &fence_));
}

Fence::~Fence() {
  vkDestroyFence(GraphicsContext::Get()->GetDevice(), fence_, nullptr);
}

Fence::Fence(Fence &&other) noexcept {
  fence_ = std::exchange(other.fence_, VK_NULL_HANDLE);
}

Fence &Fence::operator=(Fence &&other) noexcept {
  std::swap(fence_, other.fence_);
  return *this;
}

void Fence::Wait() {
  VK_CHECK(vkWaitForFences(GraphicsContext::Get()->GetDevice(), 1, &fence_, VK_TRUE, UINT64_MAX));
}

void Fence::Reset() {
  VK_CHECK(vkResetFences(GraphicsContext::Get()->GetDevice(), 1, &fence_));
}

const VkFence Fence::GetHandle() const {
  return fence_;
}

} // namespace Innsmouth