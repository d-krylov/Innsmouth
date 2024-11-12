#ifndef INNSMOUTH_GRAPHICS_TYPES_H
#define INNSMOUTH_GRAPHICS_TYPES_H

#define VK_NO_PROTOTYPES
#include "core/include/asserts.h"
#include "core/include/concepts.h"
#include "core/include/macros.h"
#include "volk.h"
#include <vector>
#include <vma/vk_mem_alloc.h>
#include <vulkan/utility/vk_format_utils.h>
#include <vulkan/vk_enum_string_helper.h>

namespace Innsmouth {

void VK_CHECK(VkResult result);

enum class CommandBufferUsage {
  ONE_TIME_SUBMIT = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  RENDER_PASS_CONTINUE = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
  SIMULTANEOUS_USE = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
};

enum class Filter { NEAREST = VK_FILTER_NEAREST, LINEAR = VK_FILTER_LINEAR, CUBIC = VK_FILTER_CUBIC_EXT };

// MESH
enum class FrontFace {
#define VULKAN_FRONT_FACE(X) X = VK_FRONT_FACE_##X,
#include "graphics_types.def"
};

enum class CullMode {
#define VULKAN_CULL_MODE(X) X = VK_CULL_MODE_##X,
#include "graphics_types.def"
};

enum class Access {
  NONE = VK_ACCESS_NONE,
#define VULKAN_ACCESS(X) X = VK_ACCESS_##X##_BIT,
#include "graphics_types.def"
};

enum class ImageAspect {
#define VULKAN_IMAGE_ASPECT(X) X = VK_IMAGE_ASPECT_##X##_BIT,
#include "graphics_types.def"
};

// QUEUE

enum class QueueFlags {
#define VULKAN_QUEUE_FLAGS(X) X = VK_QUEUE_##X##_BIT,
#include "graphics_types.def"
};

// DEBUG

enum class DebugMessageType {
#define VULKAN_DEBUG_MESSAGE_TYPE(X) X = VK_DEBUG_UTILS_MESSAGE_TYPE_##X##_BIT_EXT,
#include "graphics_types.def"
};

enum class DebugMessageSeverity {
#define VULKAN_DEBUG_MESSAGE_SEVERITY(X) X = VK_DEBUG_UTILS_MESSAGE_SEVERITY_##X##_BIT_EXT,
#include "graphics_types.def"
};

// BUFFER

enum class BufferUsage {
#define VULKAN_BUFFER_USAGE(X) X = VK_BUFFER_USAGE_##X##_BIT,
#include "graphics_types.def"
};

// PIPELINE

enum class ShaderStage {
#define VULKAN_SHADER_STAGE(X) X = VK_SHADER_STAGE_##X##_BIT,
#include "graphics_types.def"
};

enum class PipelineStage {
#define VULKAN_PIPELINE_STAGE(X) X = VK_PIPELINE_STAGE_##X##_BIT,
#include "graphics_types.def"
};

enum class BlendFactor {
#define VULKAN_BLEND_FACTOR(X) X = VK_BLEND_FACTOR_##X,
#include "graphics_types.def"
};

enum class BlendOperation {
#define VULKAN_BLEND_OPERATION(X) X = VK_BLEND_OP_##X,
#include "graphics_types.def"
};

enum class PrimitiveTopology {
#define VULKAN_PRIMITIVE_TOPOLOGY(X) X = VK_PRIMITIVE_TOPOLOGY_##X,
#include "graphics_types.def"
};

// IMAGE
enum class ImageUsage {
#define VULKAN_IMAGE_USAGE(X) X = VK_IMAGE_USAGE_##X##_BIT,
#include "graphics_types.def"
};

enum class ImageType {
#define VULKAN_IMAGE_TYPE(X) _##X = VK_IMAGE_TYPE_##X,
#include "graphics_types.def"
};

enum class ImageTiling {
#define VULKAN_IMAGE_TILING(X) X = VK_IMAGE_TILING_##X,
#include "graphics_types.def"
};

enum class ImageLayout {
#define VULKAN_IMAGE_LAYOUT(X) X = VK_IMAGE_LAYOUT_##X,
#include "graphics_types.def"
};

enum class FormatFeature {
#define VULKAN_FORMAT_FEATURE(X) X = VK_FORMAT_FEATURE_##X##_BIT,
#include "graphics_types.def"
};

enum class SampleCount {
#define VULKAN_SAMPLE_COUNT(X) BIT##X = VK_SAMPLE_COUNT_##X##_BIT,
#include "graphics_types.def"
};

enum class SamplerAddressMode {
#define VULKAN_SAMPLER_ADDRESS_MODE(X) X = VK_SAMPLER_ADDRESS_MODE_##X,
#include "graphics_types.def"
};

enum class DescriptorType {
#define VULKAN_DESCRIPTOR_TYPE(X) X = VK_DESCRIPTOR_TYPE_##X,
#include "graphics_types.def"
};

enum class CompareOperation {
#define VULKAN_COMPARE_OPERATION(X) X = VK_COMPARE_OP_##X,
#include "graphics_types.def"
};

enum class Format {
#define VULKAN_FORMAT(X) X = VK_FORMAT_##X,
#include "graphics_types.def"
};

enum class ColorSpace {
#define VULKAN_COLOR_SPACE(X) X = VK_COLOR_SPACE_##X,
#include "graphics_types.def"
};

enum class ColorComponent {
#define VULKAN_COLOR_COMPONENT(X) X = VK_COLOR_COMPONENT_##X##_BIT,
#include "graphics_types.def"
};

enum class ImageViewType {
#define VULKAN_IMAGE_VIEW_TYPE(X) _##X = VK_IMAGE_VIEW_TYPE_##X,
#include "graphics_types.def"
};

// LOAD AND STORE

enum class LoadOperation {
#define VULKAN_LOAD_OPERATION(X) X = VK_ATTACHMENT_LOAD_OP_##X,
#include "graphics_types.def"
};

enum class StoreOperation {
#define VULKAN_STORE_OPERATION(X) X = VK_ATTACHMENT_STORE_OP_##X,
#include "graphics_types.def"
};

struct RenderingAttachment {
  LoadOperation load_operation_;
  StoreOperation store_operation_;
  VkImageView image_view_;
};

ALLOW_BITMASK_ENUM(ImageUsage)
ALLOW_BITMASK_ENUM(ImageAspect)
ALLOW_BITMASK_ENUM(DebugMessageType)
ALLOW_BITMASK_ENUM(DebugMessageSeverity)
ALLOW_BITMASK_ENUM(SampleCount)
ALLOW_BITMASK_ENUM(FormatFeature)
ALLOW_BITMASK_ENUM(PipelineStage)
ALLOW_BITMASK_ENUM(ColorComponent)
ALLOW_BITMASK_ENUM(BufferUsage)

[[nodiscard]] VkAccessFlags GetAccessMask(VkImageLayout layout);
[[nodiscard]] uint32_t GetMipLevels(const VkExtent3D &e);
[[nodiscard]] VkImageSubresourceRange
CreateImageSubresourceRange(ImageAspect aspect = ImageAspect::COLOR, uint32_t base_level = 0,
                            uint32_t levels = 1, uint32_t base_layer = 0, uint32_t layers = 1);

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_TYPES_H