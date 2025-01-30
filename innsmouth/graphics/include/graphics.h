#ifndef INNSMOUTH_GRAPHICS_H
#define INNSMOUTH_GRAPHICS_H

#include "graphics_types.h"
#include <optional>

namespace Innsmouth {

struct DebugOptions {
  DebugMessageType type;
  DebugMessageSeverity severity;
};

const VkInstance Instance();
const VkPhysicalDevice PhysicalDevice();
const VkDevice Device();
const VkCommandPool CommandPool();
const VkQueue GraphicsQueue();
const VmaAllocator Allocator();
uint32_t GraphicsIndex();

void CreateGraphics(const std::optional<DebugOptions> &debug_options = std::nullopt);

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_H