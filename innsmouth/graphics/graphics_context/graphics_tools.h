#ifndef INNSMOUTH_GRAPHICS_TOOLS_H
#define INNSMOUTH_GRAPHICS_TOOLS_H

#include "innsmouth/core/include/type_tools.h"
#include "innsmouth/graphics/core/graphics_types.h"
#include <source_location>
#include <vector>

namespace Innsmouth {

template <typename T, typename FUNCTION, typename... ARGUMENTS> inline auto Enumerate(FUNCTION &&enumerate_function, ARGUMENTS &&...arguments) {
  using A = std::remove_pointer_t<decltype(&enumerate_function)>;
  using R = FunctionTraits<A>::arguments_t;
  using V = std::tuple_element_t<std::tuple_size_v<R> - 1, R>;
  auto count{0u};
  std::forward<FUNCTION>(enumerate_function)(std::forward<ARGUMENTS>(arguments)..., &count, nullptr);
  std::vector<T> ret(count);
  std::forward<FUNCTION>(enumerate_function)(std::forward<ARGUMENTS>(arguments)..., &count, reinterpret_cast<V>(ret.data()));
  return ret;
}

void VK_CHECK(VkResult result, std::source_location = std::source_location::current());

bool EvaluatePhysicalDevice(const VkPhysicalDevice physical_device);

int32_t PickPhysicalDeviceQueue(const VkPhysicalDevice physical_device);

std::vector<const char *> GetRequiredDeviceExtensions();

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_TOOLS_H