#ifndef INNSMOUTH_GRAPHICS_HELPERS_H
#define INNSMOUTH_GRAPHICS_HELPERS_H

#include "graphics_types.h"
#include <ranges>

namespace Innsmouth {

template <typename T, typename F, typename... ARGS>
[[nodiscard]] auto Enumerate(F enumerate_function, ARGS &&...arguments) -> std::vector<T> {
  uint32_t count{0};
  enumerate_function(std::forward<ARGS>(arguments)..., &count, nullptr);
  std::vector<T> ret(count);
  enumerate_function(std::forward<ARGS>(arguments)..., &count, ret.data());
  return ret;
}

[[nodiscard]] bool GetSupportedFeatures(const VkPhysicalDevice, const VkPhysicalDeviceFeatures &);
[[nodiscard]] bool GetSupportedExtensions(const VkPhysicalDevice, const std::vector<const char *> &);
[[nodiscard]] std::optional<uint32_t> GetQueueFamily(const VkPhysicalDevice, VkQueueFlags);
[[nodiscard]] VkFormat FindSupportedFormat(const std::ranges::input_range auto &formats, ImageTiling tiling,
                                           VkFormatFeatureFlags requiered_features);

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_HELPERS_H