#include "descriptor_set.h"
#include "innsmouth/graphics/image/image.h"

namespace Innsmouth {

DescriptorSet::DescriptorSet(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout set_layout, uint32_t descriptors_count) {
  DescriptorSetVariableDescriptorCountAllocateInfo descriptor_set_count_ai{};
  {
    descriptor_set_count_ai.descriptorSetCount = 1;
    descriptor_set_count_ai.pDescriptorCounts = &descriptors_count;
  }

  DescriptorSetAllocateInfo descriptor_set_ai{};
  {
    descriptor_set_ai.pNext = &descriptor_set_count_ai;
    descriptor_set_ai.descriptorPool = descriptor_pool;
    descriptor_set_ai.descriptorSetCount = 1;
    descriptor_set_ai.pSetLayouts = &set_layout;
  }

  VK_CHECK(vkAllocateDescriptorSets(GraphicsContext::Get()->GetDevice(), descriptor_set_ai, &descriptor_set_));
}

DescriptorSet::~DescriptorSet() {
}

void DescriptorSet::Update(uint32_t binding) {

  DescriptorImageInfo descriptor_ii{};
  {
    // descriptor_ii.imageView = images[i].image_view_;
    // descriptor_ii.sampler = images[i].sampler_;
    descriptor_ii.imageLayout = ImageLayout::E_GENERAL;
  }

  WriteDescriptorSet write_descriptor_set{};
  {
    write_descriptor_set.dstSet = GetHandle();
    write_descriptor_set.dstBinding = binding;
    // write_descriptor_set.dstArrayElement = i;
    // write_descriptor_set.descriptorCount = 1;
    write_descriptor_set.descriptorType = DescriptorType::E_COMBINED_IMAGE_SAMPLER;
    write_descriptor_set.pImageInfo = &descriptor_ii;
  }

  vkUpdateDescriptorSets(GraphicsContext::Get()->GetDevice(), 1, write_descriptor_set, 0, nullptr);
}

VkDescriptorSet DescriptorSet::GetHandle() const {
  return descriptor_set_;
}

} // namespace Innsmouth