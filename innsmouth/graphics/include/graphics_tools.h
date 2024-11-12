#ifndef INNSMOUTH_GRAPHICS_TOOLS_H
#define INNSMOUTH_GRAPHICS_TOOLS_H

#include "graphics_types.h"
#include <span>

namespace Innsmouth {

template <typename T, typename F, typename... ARGS>
[[nodiscard]] auto Enumerate(F enumerate_function, ARGS &&...arguments) -> std::vector<T> {
  uint32_t count{0};
  enumerate_function(std::forward<ARGS>(arguments)..., &count, nullptr);
  std::vector<T> ret(count);
  enumerate_function(std::forward<ARGS>(arguments)..., &count, ret.data());
  return ret;
}

struct DeviceFeatures {

  DeviceFeatures() { Set(); }

  DeviceFeatures(const VkPhysicalDeviceFeatures2 &features,
                 const VkPhysicalDeviceVulkan11Features &features_1_1,
                 const VkPhysicalDeviceVulkan12Features &features_1_2,
                 const VkPhysicalDeviceVulkan13Features &features_1_3,
                 const VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamic_state_features)
    : features_(features), features_1_1_(features_1_1), features_1_2_(features_1_2),
      features_1_3_(features_1_3), dynamic_state_features_(dynamic_state_features) {
    Set();
  }

  void Set() {
    features_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features_.pNext = &features_1_1_;
    features_1_1_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    features_1_1_.pNext = &features_1_2_;
    features_1_2_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features_1_2_.pNext = &features_1_3_;
    features_1_3_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features_1_3_.pNext = &dynamic_state_features_;
    dynamic_state_features_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
    dynamic_state_features_.pNext = nullptr;
  }

  VkPhysicalDeviceFeatures2 features_{};
  VkPhysicalDeviceVulkan11Features features_1_1_{};
  VkPhysicalDeviceVulkan12Features features_1_2_{};
  VkPhysicalDeviceVulkan13Features features_1_3_{};
  VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamic_state_features_{};
};

[[nodiscard]] VkPhysicalDevice PickPhysicalDevice(std::span<const VkPhysicalDevice> physical_devices);
[[nodiscard]] VkPhysicalDeviceFeatures2 GetRequiredFeatures();
[[nodiscard]] VkPhysicalDeviceVulkan11Features GetRequiredFeatures11();
[[nodiscard]] VkPhysicalDeviceVulkan12Features GetRequiredFeatures12();
[[nodiscard]] VkPhysicalDeviceVulkan13Features GetRequiredFeatures13();
[[nodiscard]] VkPhysicalDeviceExtendedDynamicStateFeaturesEXT GetExtendedDynamicState();
[[nodiscard]] std::vector<const char *> GetRequiredDeviceExtensions();
[[nodiscard]] std::optional<uint32_t> GetQueueFamilyIndex(std::span<const VkQueueFamilyProperties> properties,
                                                          QueueFlags flags);

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_TOOLS_H