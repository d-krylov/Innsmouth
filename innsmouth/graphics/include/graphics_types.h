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
enum class DebugMessageType : VkDebugUtilsMessageTypeFlagsEXT {
#define VULKAN_DEBUG_MESSAGE_TYPE(X) X = VK_DEBUG_UTILS_MESSAGE_TYPE_##X##_BIT_EXT,
#include "graphics_types.def"
};

enum class DebugMessageSeverity : VkDebugUtilsMessageSeverityFlagsEXT {
#define VULKAN_DEBUG_MESSAGE_SEVERITY(X) X = VK_DEBUG_UTILS_MESSAGE_SEVERITY_##X##_BIT_EXT,
#include "graphics_types.def"
};

// QUEUE

enum class QueueMask {
#define VULKAN_QUEUE_MASK(X) X = VK_QUEUE_##X##_BIT,
#include "graphics_types.def"
};

// BUFFER
enum class BufferUsage : VkBufferUsageFlags {
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

ALLOW_BITMASK_ENUM(QueueMask);
ALLOW_BITMASK_ENUM(DebugMessageType);
ALLOW_BITMASK_ENUM(DebugMessageSeverity);
ALLOW_BITMASK_ENUM(MemoryProperty);
ALLOW_BITMASK_ENUM(ColorComponent);

// FUNCTIONS
void VK_CHECK(VkResult result, std::source_location = std::source_location::current());

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_TYPES_H