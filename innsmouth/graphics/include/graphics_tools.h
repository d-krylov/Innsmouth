#ifndef INNSMOUTH_GRAPHICS_TOOLS_H
#define INNSMOUTH_GRAPHICS_TOOLS_H

#include "core/include/type_tools.h"
#include "graphics_types.h"
#include <vector>
#include <span>
#include <optional>

namespace Innsmouth {

template <typename F, typename... ARGS> inline auto Enumerate(F &&enumerate_function, ARGS &&...arguments) {
  using R = FunctionTraits<std::remove_pointer_t<decltype(&enumerate_function)>>::arguments_t;
  using V = std::remove_pointer_t<std::tuple_element_t<std::tuple_size_v<R> - 1, R>>;
  auto count{0u};
  std::forward<F>(enumerate_function)(std::forward<ARGS>(arguments)..., &count, nullptr);
  std::vector<V> ret(count);
  std::forward<F>(enumerate_function)(std::forward<ARGS>(arguments)..., &count, ret.data());
  return ret;
}

struct QueueIndices {
  std::optional<int32_t> graphics_;
  std::optional<int32_t> compute_;
  std::optional<int32_t> transfer_;
};

std::span<const char *> GetRequiredDeviceExtensions();

VkPhysicalDeviceFeatures2 GetRequredDeviceFeatures();
VkPhysicalDeviceVulkan11Features GetRequredDeviceFeatures11();
VkPhysicalDeviceVulkan12Features GetRequredDeviceFeatures12();
VkPhysicalDeviceVulkan13Features GetRequredDeviceFeatures13();
QueueIndices GetPhysicalDeviceQueueIndices(const VkPhysicalDevice physical_device);

VkPhysicalDevice PickPhysicalDevice(std::span<const VkPhysicalDevice> physical_devices);

struct DeviceFeatures {
  DeviceFeatures() { Set(); }

  DeviceFeatures(auto &&features, auto &&features11, auto &&features12, auto &&features13)
    : features_(features), features_1_1_(features11), features_1_2_(features12), features_1_3_(features13) {
    Set();
  }

  void Set() {
    features_.pNext = &features_1_1_;
    features_1_1_.pNext = &features_1_2_;
    features_1_2_.pNext = &features_1_3_;
    features_1_3_.pNext = nullptr;
  }

  VkPhysicalDeviceFeatures2 features_{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
  VkPhysicalDeviceVulkan11Features features_1_1_{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
  VkPhysicalDeviceVulkan12Features features_1_2_{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
  VkPhysicalDeviceVulkan13Features features_1_3_{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
};

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_TOOLS_H