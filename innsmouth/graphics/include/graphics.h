#ifndef INNSMOUTH_GRAPHICS_H
#define INNSMOUTH_GRAPHICS_H

#include "innsmouth/graphics/include/graphics_description.h"
#include <optional>

namespace Innsmouth {

template <typename T, typename F, typename... ARGS>
[[nodiscard]] auto Enumerate(F enumerate_function, ARGS &&...arguments) -> std::vector<T> {
  uint32_t count{0};
  enumerate_function(std::forward<ARGS>(arguments)..., &count, nullptr);
  std::vector<T> ret(count);
  enumerate_function(std::forward<ARGS>(arguments)..., &count, ret.data());
  return ret;
}

[[nodiscard]] const VkInstance Instance();
[[nodiscard]] const VkPhysicalDevice PhysicalDevice();
[[nodiscard]] const VkDevice Device();
[[nodiscard]] const VkCommandPool CommandPool();
[[nodiscard]] const VkQueue GraphicsQueue();
[[nodiscard]] const VmaAllocator Allocator();
[[nodiscard]] uint32_t GraphicsIndex();

void CreateGraphics(const GraphicsDescription &graphics_description);
void DestroyGraphics();

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_H