#ifndef INNSMOUTH_GRAPHICS_TYPES_H
#define INNSMOUTH_GRAPHICS_TYPES_H

#define VK_NO_PROTOTYPES
#include "core/include/asserts.h"
#include "core/include/macros.h"
#include "volk/volk.h"
#include <optional>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vk_format_utils.h>

namespace Innsmouth {

void VK_CHECK(VkResult result);

enum class DebugMessageType {
  NONE = 0,
  GENERAL = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
  VALIDATION = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
  PERFORMANCE = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
  GENERAL_AND_VALIDATION = GENERAL | VALIDATION,
  ALL = GENERAL | VALIDATION | PERFORMANCE
};

enum class DebugMessageSeverity {
  VERBOSE = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
  INFO = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
  WARNING = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
  ERROR = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
  VERBOSE_ERROR_AND_WARNING = VERBOSE | WARNING | ERROR,
  ALL = VERBOSE | INFO | WARNING | ERROR
};

enum class CommandBufferUsage {
  ONE_TIME_SUBMIT = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  RENDER_PASS_CONTINUE = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
  SIMULTANEOUS_USE = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
};

enum class BufferUsage {
#define VULKAN_BUFFER_USAGE(X) X = VK_BUFFER_USAGE_##X##_BIT,
#include "graphics_types.def"
};

struct WriteDescriptorSet {
  WriteDescriptorSet(const VkDescriptorBufferInfo &buffer_info,
                     const VkWriteDescriptorSet &write_descriptor_set)
    : buffer_info_(std::make_unique<VkDescriptorBufferInfo>(buffer_info)),
      write_descriptor_set_(write_descriptor_set) {
    write_descriptor_set_.pBufferInfo = buffer_info_.get();
  }

  WriteDescriptorSet(const VkDescriptorImageInfo &image_info,
                     const VkWriteDescriptorSet &write_descriptor_set)
    : image_info_(std::make_unique<VkDescriptorImageInfo>(image_info)),
      write_descriptor_set_(write_descriptor_set) {
    write_descriptor_set_.pImageInfo = image_info_.get();
  }

  VkWriteDescriptorSet write_descriptor_set_;
  std::unique_ptr<VkDescriptorImageInfo> image_info_;
  std::unique_ptr<VkDescriptorBufferInfo> buffer_info_;
};

[[nodiscard]] VkAccessFlags GetAccessMask(VkImageLayout layout);

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_TYPES_H