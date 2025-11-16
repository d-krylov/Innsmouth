#ifndef INNSMOUTH_DESCRIPTOR_SET_H
#define INNSMOUTH_DESCRIPTOR_SET_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"
#include <span>

namespace Innsmouth {

class Image;

class DescriptorSet {
public:
  DescriptorSet(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout set_layout, uint32_t descriptors_count);

  ~DescriptorSet();

  void Update(uint32_t binding);

  VkDescriptorSet GetHandle() const;

private:
  VkDescriptorSet descriptor_set_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_DESCRIPTOR_SET_H