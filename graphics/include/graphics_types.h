#ifndef INNSMOUTH_GRAPHICS_TYPES_H
#define INNSMOUTH_GRAPHICS_TYPES_H

#define VK_NO_PROTOTYPES
#include "core/include/asserts.h"
#include "core/include/macros.h"
#include "volk/volk.h"
#include <optional>
#include <vector>
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

enum class ImageAspect {
  COLOR = VK_IMAGE_ASPECT_COLOR_BIT,
  DEPTH = VK_IMAGE_ASPECT_DEPTH_BIT,
  STENCIL = VK_IMAGE_ASPECT_STENCIL_BIT
};

enum class FrontFace {
  COUNTER_CLOCKWISE = VK_FRONT_FACE_COUNTER_CLOCKWISE,
  CLOCKWISE = VK_FRONT_FACE_CLOCKWISE
};

enum class Filter {
  NEAREST = VK_FILTER_NEAREST,
  LINEAR = VK_FILTER_LINEAR,
  CUBIC = VK_FILTER_CUBIC_EXT
};

enum class BufferUsage {
#define VULKAN_BUFFER_USAGE(X) X = VK_BUFFER_USAGE_##X##_BIT,
#include "graphics_types.def"
};

enum class ImageUsage {
#define VULKAN_IMAGE_USAGE(X) X = VK_IMAGE_USAGE_##X##_BIT,
#include "graphics_types.def"
  SAMPLED_TRANFER_DST = TRANSFER_DST | SAMPLED,
  SAMPLED_DEPTH_STENCIL_ATTACHMENT = DEPTH_STENCIL_ATTACHMENT | SAMPLED
};

enum class ShaderStage {
#define VULKAN_SHADER_STAGE(X) X = VK_SHADER_STAGE_##X##_BIT,
#include "graphics_types.def"
};

enum class ImageLayout {
#define VULKAN_IMAGE_LAYOUT(X) X = VK_IMAGE_LAYOUT_##X,
#include "graphics_types.def"
  PRESENT_SRC_KHR = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
};

enum class PipelineStage {
#define VULKAN_PIPELINE_STAGE(X) X = VK_PIPELINE_STAGE_##X##_BIT,
#include "graphics_types.def"
  NONE = VK_PIPELINE_STAGE_NONE
};

enum class SamplerAddressMode {
#define VULKAN_SAMPLER_ADDRESS_MODE(X) X = VK_SAMPLER_ADDRESS_MODE_##X,
#include "graphics_types.def"
};

enum class DescriptorType {
#define VULKAN_DESCRIPTOR_TYPE(X) X = VK_DESCRIPTOR_TYPE_##X,
#include "graphics_types.def"
};

enum class Format {
#define VULKAN_FORMAT(X) X = VK_FORMAT_##X,
#include "graphics_types.def"
};

enum class LoadOperation {
  LOAD = VK_ATTACHMENT_LOAD_OP_LOAD,
  CLAER = VK_ATTACHMENT_LOAD_OP_CLEAR,
  DONT_CARE = VK_ATTACHMENT_LOAD_OP_DONT_CARE
};

enum class Depth { NONE, READ, WRITE, READ_WRITE };

enum class PrimitiveTopology {
#define VULKAN_PRIMITIVE_TOPOLOGY(X) X = VK_PRIMITIVE_TOPOLOGY_##X,
#include "graphics_types.def"
};

struct RenderingAttachment {
  LoadOperation load_operation_;
  VkImageView image_view_;
};

[[nodiscard]] std::vector<VkDynamicState> GetDynamicStates();
[[nodiscard]] VkAccessFlags GetAccessMask(VkImageLayout layout);
[[nodiscard]] uint32_t GetMipLevels(const VkExtent3D &e);
[[nodiscard]] VkImageSubresourceRange
CreateImageSubresourceRange(ImageAspect aspect = ImageAspect::COLOR, uint32_t base_level = 0,
                            uint32_t levels = 1, uint32_t base_layer = 0, uint32_t layers = 1);

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_TYPES_H