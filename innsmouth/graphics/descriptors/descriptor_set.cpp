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

DescriptorSet::DescriptorSet(DescriptorSet &&other) noexcept {
  descriptor_set_ = std::exchange(other.descriptor_set_, VK_NULL_HANDLE);
}

DescriptorSet &DescriptorSet::operator=(DescriptorSet &&other) noexcept {
  std::swap(descriptor_set_, other.descriptor_set_);
  return *this;
}

void DescriptorSet::Update(std::span<const DescriptorImageInfo> images, uint32_t binding, DescriptorType descriptor_type, uint32_t start) {
  WriteDescriptorSet write_descriptor_set;
  write_descriptor_set.dstSet = descriptor_set_;
  write_descriptor_set.dstBinding = binding;
  write_descriptor_set.dstArrayElement = start;
  write_descriptor_set.descriptorType = descriptor_type;
  write_descriptor_set.descriptorCount = images.size();
  write_descriptor_set.pImageInfo = images.data();
  vkUpdateDescriptorSets(GraphicsContext::Get()->GetDevice(), 1, write_descriptor_set, 0, nullptr);
}

VkDescriptorSet DescriptorSet::GetHandle() const {
  return descriptor_set_;
}

} // namespace Innsmouth