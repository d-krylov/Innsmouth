#ifndef INNSMOUTH_GRAPHICS_TOOLS_H
#define INNSMOUTH_GRAPHICS_TOOLS_H

#include "innsmouth/core/include/type_tools.h"
#include "innsmouth/graphics/core/graphics_types.h"
#include <source_location>
#include <vector>

namespace Innsmouth {

template <typename FUNCTION, typename... ARGUMENTS> inline auto Enumerate(FUNCTION &&enumerate_function, ARGUMENTS &&...arguments) {
  using R = FunctionTraits<std::remove_pointer_t<decltype(&enumerate_function)>>::arguments_t;
  using V = std::remove_pointer_t<std::tuple_element_t<std::tuple_size_v<R> - 1, R>>;
  auto count{0u};
  std::forward<FUNCTION>(enumerate_function)(std::forward<ARGUMENTS>(arguments)..., &count, nullptr);
  std::vector<V> ret(count);
  std::forward<FUNCTION>(enumerate_function)(std::forward<ARGUMENTS>(arguments)..., &count, ret.data());
  return ret;
}

void VK_CHECK(VkResult result, std::source_location = std::source_location::current());

bool EvaluatePhysicalDevice(const VkPhysicalDevice physical_device);

struct QueueIndices {
  int32_t general{-1};
  int32_t compute{-1};
  int32_t transfer{-1};
};

QueueIndices PickPhysicalDeviceQueues(const VkPhysicalDevice physical_device);
std::vector<const char *> GetRequiredDeviceExtensions();

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_TOOLS_H