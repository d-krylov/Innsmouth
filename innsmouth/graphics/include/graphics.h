#ifndef INNSMOUTH_GRAPHICS_H
#define INNSMOUTH_GRAPHICS_H

#include "graphics_types.h"
#include <optional>
#include <vector>

namespace Innsmouth {

struct DebugOptions {
  DebugMessageType type;
  DebugMessageSeverity severity;
};

const VkInstance Instance();
const VkPhysicalDevice PhysicalDevice();
const VkDevice Device();
const VkCommandPool GraphicsCommandPool();
const VkCommandPool ComputeCommandPool();
const VkCommandPool TransferCommandPool();
const VkQueue GraphicsQueue();
const VkQueue TransferQueue();
const VkQueue ComputeQueue();
const VmaAllocator Allocator();
uint32_t GraphicsIndex();
void WaitIdle();

struct Graphics {
  Graphics(const std::vector<const char *> &extensions, const std::optional<DebugOptions> &debug_options);
  ~Graphics();
};

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_H