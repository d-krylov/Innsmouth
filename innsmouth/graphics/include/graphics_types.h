#ifndef INNSMOUTH_GRAPHICS_TYPES_H
#define INNSMOUTH_GRAPHICS_TYPES_H

#include "core/include/concept_tools.h"
#define VK_NO_PROTOTYPES
#include "vma/vk_mem_alloc.h"
#include "volk.h"
#include <vulkan/utility/vk_format_utils.h>
#include <source_location>

namespace Innsmouth {

// ENUMS

enum class Format {
#define VULKAN_FORMAT(X) X = VK_FORMAT_##X,
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

// QUEUE

enum class QueueMask {
#define VULKAN_QUEUE_MASK(X) X = VK_QUEUE_##X##_BIT,
#include "graphics_types.def"
};

// BUFFER
enum class BufferUsage {
#define VULKAN_BUFFER_USAGE(X) X = VK_BUFFER_USAGE_##X,
#include "graphics_types.def"
};

enum class CommandBufferUsage {
#define VULKAN_COMMAND_BUFFER_USAGE(X) X = VK_COMMAND_BUFFER_USAGE_##X##_BIT,
#include "graphics_types.def"
};

enum class ShaderStage {
#define VULKAN_SHADER_STAGE(X) X = VK_SHADER_STAGE_##X,
#include "graphics_types.def"
};

enum class ImageViewType {
#define VULKAN_IMAGE_VIEW_TYPE(X) _##X = VK_IMAGE_VIEW_TYPE_##X,
#include "graphics_types.def"
};

enum class ImageAspect {
#define VULKAN_IMAGE_ASPECT(X) X = VK_IMAGE_ASPECT_##X,
#include "graphics_types.def"
};

enum class ImageLayout {
#define VULKAN_IMAGE_LAYOUT(X) X = VK_IMAGE_LAYOUT_##X,
#include "graphics_types.def"
};

enum class DependencyBit {
#define VULKAN_DEPENDENCY_BIT(X) X = VK_DEPENDENCY_##X,
#include "graphics_types.def"
};

enum class AccessMask : VkAccessFlags2 {
#define VULKAN_ACCESS_MASK(X) X = VK_ACCESS_2_##X,
#include "graphics_types.def"
};

enum class PipelineStage : VkPipelineStageFlags2 {
#define VULKAN_PIPELINE_STAGE(X) X = VK_PIPELINE_STAGE_2_##X,
#include "graphics_types.def"
};

enum class MemoryProperty {
#define VULKAN_MEMORY_PROPERTY(X) X = VK_MEMORY_PROPERTY_##X,
#include "graphics_types.def"
};

enum class MemoryUsage {
#define VMA_MEMORY_USAGE(X) X = VMA_MEMORY_USAGE_##X,
#include "graphics_types.def"
};

enum class ColorComponent {
#define VULKAN_COLOR_COMPONENT(X) X = VK_COLOR_COMPONENT_##X##_BIT,
#include "graphics_types.def"
};

enum class DescriptorType {
#define VULKAN_DESCRIPTOR_TYPE(X) X = VK_DESCRIPTOR_TYPE_##X,
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

enum class Filter {
#define VULKAN_FILTER(X) X = VK_FILTER_##X,
#include "graphics_types.def"
};

enum class LoadOperation {
#define VULKAN_LOAD_OPERATION(X) X = VK_ATTACHMENT_LOAD_OP_##X,
#include "graphics_types.def"
};

enum class StoreOperation {
#define VULKAN_STORE_OPERATION(X) X = VK_ATTACHMENT_STORE_OP_##X,
#include "graphics_types.def"
};

enum class CullMode {
#define VULKAN_CULL_MODE(X) X = VK_CULL_MODE_##X,
#include "graphics_types.def"
};

enum class FrontFace {
#define VULKAN_FRONT_FACE(X) X = VK_FRONT_FACE_##X,
#include "graphics_types.def"
};

enum class CompareOperation {
#define VULKAN_COMPARE_OPERATION(X) X = VK_COMPARE_OP_##X,
#include "graphics_types.def"
};

enum class ImageUsage {
#define VULKAN_IMAGE_USAGE(X) X = VK_IMAGE_USAGE_##X,
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

enum class SampleCount {
#define VULKAN_SAMPLE_COUNT(X) BIT##X = VK_SAMPLE_COUNT_##X##_BIT,
#include "graphics_types.def"
};

enum class SamplerAddressMode {
#define VULKAN_SAMPLER_ADDRESS_MODE(X) X = VK_SAMPLER_ADDRESS_MODE_##X,
#include "graphics_types.def"
};

ALLOW_BITMASK_ENUM(QueueMask);
ALLOW_BITMASK_ENUM(DebugMessageType);
ALLOW_BITMASK_ENUM(DebugMessageSeverity);
ALLOW_BITMASK_ENUM(MemoryProperty);
ALLOW_BITMASK_ENUM(ColorComponent);
ALLOW_BITMASK_ENUM(CullMode);
ALLOW_BITMASK_ENUM(ImageUsage);
ALLOW_BITMASK_ENUM(SampleCount);

struct SamplerAddress {
  SamplerAddress(SamplerAddressMode x) : SamplerAddress(x, x, x) {}
  SamplerAddress(SamplerAddressMode u, SamplerAddressMode v, SamplerAddressMode w) : u_(u), v_(v), w_(w) {}
  SamplerAddressMode u_;
  SamplerAddressMode v_;
  SamplerAddressMode w_;
};

// FUNCTIONS
void VK_CHECK(VkResult result, std::source_location = std::source_location::current());

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_TYPES_H