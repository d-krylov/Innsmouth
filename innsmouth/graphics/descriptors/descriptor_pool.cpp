#include "descriptor_pool.h"

namespace Innsmouth {

DescriptorPool::DescriptorPool(std::span<const DescriptorPoolSize> descriptor_pool_sizes, DescriptorPoolCreateMask mask, uint32_t max_sets) {
  DescriptorPoolCreateInfo descriptor_pool_ci;
  {
    descriptor_pool_ci.flags = mask;
    descriptor_pool_ci.maxSets = max_sets;
    descriptor_pool_ci.poolSizeCount = descriptor_pool_sizes.size();
    descriptor_pool_ci.pPoolSizes = descriptor_pool_sizes.data();
  }
  VK_CHECK(vkCreateDescriptorPool(GraphicsContext::Get()->GetDevice(), descriptor_pool_ci, 0, &descriptor_pool_));
}

DescriptorPool::~DescriptorPool() {
}

VkDescriptorPool DescriptorPool::GetHandle() const {
  return descriptor_pool_;
}

DescriptorPool::DescriptorPool(DescriptorPool &&other) noexcept {
  descriptor_pool_ = std::exchange(other.descriptor_pool_, VK_NULL_HANDLE);
}

DescriptorPool &DescriptorPool::operator=(DescriptorPool &&other) noexcept {
  std::swap(descriptor_pool_, other.descriptor_pool_);
  return *this;
}

} // namespace Innsmouth