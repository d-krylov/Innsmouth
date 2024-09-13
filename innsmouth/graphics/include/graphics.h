#ifndef INNSMOUTH_GRAPHICS_H
#define INNSMOUTH_GRAPHICS_H

#include "innsmouth/graphics/include/graphics_description.h"
#include <optional>

namespace Innsmouth {

[[nodiscard]] const VkInstance Instance();
[[nodiscard]] const VkPhysicalDevice PhysicalDevice();
[[nodiscard]] const VkDevice Device();
[[nodiscard]] const VkCommandPool CommandPool();
[[nodiscard]] const VkQueue GraphicsQueue();
[[nodiscard]] const VmaAllocator Allocator();
[[nodiscard]] uint32_t GraphicsIndex();

void WaitIdle();

class Graphics {
public:
  Graphics(const GraphicsDescription &graphics_description);

  ~Graphics();

private:
};

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_H