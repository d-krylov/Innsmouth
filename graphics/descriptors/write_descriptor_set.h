#ifndef INNSMOUTH_WRITE_DESCRIPTOR_SET_H
#define INNSMOUTH_WRITE_DESCRIPTOR_SET_H

#include "graphics/include/graphics_types.h"

namespace Innsmouth {

struct WriteDescriptorSet {

  WriteDescriptorSet() = default;

  // NO_COPY_SEMANTIC(WriteDescriptorSet);

  WriteDescriptorSet(const VkDescriptorBufferInfo &buffer_info,
                     const VkWriteDescriptorSet &write_descriptor_set)
    : buffer_info_{buffer_info}, write_descriptor_set_(write_descriptor_set) {
    write_descriptor_set_.pBufferInfo = buffer_info_.data();
  }

  WriteDescriptorSet(const VkDescriptorImageInfo &image_info,
                     const VkWriteDescriptorSet &write_descriptor_set)
    : image_info_{image_info}, write_descriptor_set_(write_descriptor_set) {
    write_descriptor_set_.pImageInfo = image_info_.data();
  }

  WriteDescriptorSet(const std::vector<VkDescriptorImageInfo> &image_info,
                     const VkWriteDescriptorSet &write_descriptor_set)
    : image_info_(image_info), write_descriptor_set_(write_descriptor_set) {
    write_descriptor_set_.pImageInfo = image_info_.data();
  }

  VkWriteDescriptorSet write_descriptor_set_;
  std::vector<VkDescriptorImageInfo> image_info_;
  std::vector<VkDescriptorBufferInfo> buffer_info_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_WRITE_DESCRIPTOR_SET_H