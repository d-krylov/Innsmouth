#ifndef INNSMOUTH_DESCRIPTOR_POOL_H
#define INNSMOUTH_DESCRIPTOR_POOL_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"
#include <span>

namespace Innsmouth {

class DescriptorPool {
public:
  DescriptorPool(std::span<const DescriptorPoolSize> descriptor_pool_sizes, DescriptorPoolCreateMask mask, uint32_t max_sets);

  ~DescriptorPool();

  VkDescriptorPool GetHandle() const;

private:
  VkDescriptorPool descriptor_pool_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_DESCRIPTOR_POOL_H