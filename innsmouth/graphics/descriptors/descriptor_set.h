#ifndef INNSMOUTH_DESCRIPTOR_SET_H
#define INNSMOUTH_DESCRIPTOR_SET_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"
#include <span>

namespace Innsmouth {

class DescriptorSet {
public:
  DescriptorSet() = default;

  DescriptorSet(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout set_layout, uint32_t descriptors_count);

  ~DescriptorSet();

  DescriptorSet(const DescriptorSet &) = delete;
  DescriptorSet &operator=(const DescriptorSet &) = delete;

  DescriptorSet(DescriptorSet &&other) noexcept;
  DescriptorSet &operator=(DescriptorSet &&other) noexcept;

  void Update(std::span<const DescriptorImageInfo> images, uint32_t binding, DescriptorType descriptor_type, uint32_t start);

  VkDescriptorSet GetHandle() const;

private:
  VkDescriptorSet descriptor_set_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_DESCRIPTOR_SET_H