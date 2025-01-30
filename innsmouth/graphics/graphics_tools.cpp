#include "graphics/include/graphics_tools.h"
#include <algorithm>
#include <iostream>
#include <ranges>

namespace Innsmouth {

std::vector<const char *> GetRequiredDeviceExtensions() {
  return std::vector{VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
                     VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
                     VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
                     VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
                     VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME,
                     VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
                     VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

VkPhysicalDeviceFeatures2 GetRequredDeviceFeatures() {
  VkPhysicalDeviceFeatures2 required{};
  required.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  required.features.geometryShader = VK_TRUE;
  required.features.tessellationShader = VK_TRUE;
  required.features.multiDrawIndirect = VK_TRUE;
  return required;
}

VkPhysicalDeviceVulkan11Features GetRequredDeviceFeatures11() {
  VkPhysicalDeviceVulkan11Features required{};
  required.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
  return required;
}

VkPhysicalDeviceVulkan12Features GetRequredDeviceFeatures12() {
  VkPhysicalDeviceVulkan12Features required{};
  required.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
  required.descriptorIndexing = VK_TRUE;
  return required;
}

VkPhysicalDeviceVulkan13Features GetRequredDeviceFeatures13() {
  VkPhysicalDeviceVulkan13Features required{};
  required.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
  required.dynamicRendering = VK_TRUE;
  required.synchronization2 = VK_TRUE;
  return required;
}

bool CheckSupportedExtensions(std::span<const VkExtensionProperties> &supported, std::span<const char *> &required) {
  auto supported_names = std::views::transform(supported, [](const auto &property) { return std::string_view(property.extensionName); });
  return true;
}

template <typename T> bool Compare(const T &supported, const T &required, VkBool32 T::*field, int32_t size) {
  std::span<const VkBool32> S(&(supported.*field), size);
  std::span<const VkBool32> R(&(required.*field), size);
  return std::ranges::equal(R, S, [](auto r, auto s) { return !r || s; });
}

bool Compare(const VkPhysicalDeviceFeatures2 &supported, const VkPhysicalDeviceFeatures2 &required) {
  constexpr auto n = sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32);
  auto S = std::bit_cast<std::array<VkBool32, n>>(supported.features);
  auto R = std::bit_cast<std::array<VkBool32, n>>(required.features);
  return std::ranges::equal(R, S, [](auto r, auto s) { return !r || s; });
}

bool Compare(const VkPhysicalDeviceVulkan11Features &supported, const VkPhysicalDeviceVulkan11Features &required) {
  auto offset_0 = offsetof(VkPhysicalDeviceVulkan11Features, storageBuffer16BitAccess);
  auto offset_1 = offsetof(VkPhysicalDeviceVulkan11Features, shaderDrawParameters);
  auto size = 1 + (offset_1 - offset_0) / sizeof(VkBool32);
  return Compare(supported, required, &VkPhysicalDeviceVulkan11Features::storageBuffer16BitAccess, size);
}

bool Compare(const VkPhysicalDeviceVulkan12Features &supported, const VkPhysicalDeviceVulkan12Features &required) {
  auto offset_0 = offsetof(VkPhysicalDeviceVulkan12Features, samplerMirrorClampToEdge);
  auto offset_1 = offsetof(VkPhysicalDeviceVulkan12Features, subgroupBroadcastDynamicId);
  auto size = 1 + (offset_1 - offset_0) / sizeof(VkBool32);
  return Compare(supported, required, &VkPhysicalDeviceVulkan12Features::samplerMirrorClampToEdge, size);
}

bool Compare(const VkPhysicalDeviceVulkan13Features &supported, const VkPhysicalDeviceVulkan13Features &required) {
  auto offset_0 = offsetof(VkPhysicalDeviceVulkan13Features, robustImageAccess);
  auto offset_1 = offsetof(VkPhysicalDeviceVulkan13Features, maintenance4);
  auto size = 1 + (offset_1 - offset_0) / sizeof(VkBool32);
  return Compare(supported, required, &VkPhysicalDeviceVulkan13Features::robustImageAccess, size);
}

bool CheckPhysicalDevice(const VkPhysicalDevice physical_device) {
  DeviceFeatures device_features;
  VkPhysicalDeviceProperties device_properties{};

  vkGetPhysicalDeviceFeatures2(physical_device, &device_features.features_);
  vkGetPhysicalDeviceProperties(physical_device, &device_properties);

  bool b = Compare(device_features.features_, GetRequredDeviceFeatures()) &
           Compare(device_features.features_1_2_, GetRequredDeviceFeatures12()) &
           Compare(device_features.features_1_3_, GetRequredDeviceFeatures13());

  b &= (device_properties.apiVersion >= VK_API_VERSION_1_3);
  b &= (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);

  return b;
}

void GetPhysicalDeviceQueueIndices(const VkPhysicalDevice physical_device) {
  auto properties = Enumerate(vkGetPhysicalDeviceQueueFamilyProperties, physical_device);
  auto cast_qbits = [](auto property) { return static_cast<QueueMask>(property.queueFlags); };
  auto graphics = -1, compute = -1, transfer = -1;
  std::ranges::for_each(properties | std::views::transform(cast_qbits) | std::views::enumerate, [&](auto &&indexed_bits) {
    auto [i, bit] = indexed_bits;
    if (HasBits(bit, QueueMask::GRAPHICS)) {
      graphics = i;
    } else if (HasBits(bit, QueueMask::COMPUTE) && !HasBits(bit, QueueMask::GRAPHICS)) {
      compute = i;
    } else if (HasBits(bit, QueueMask::TRANSFER) && !HasBits(bit, QueueMask::GRAPHICS | QueueMask::COMPUTE)) {
      transfer = i;
    }
  });
}

VkPhysicalDevice PickPhysicalDevice(std::span<const VkPhysicalDevice> physical_devices) {
  auto required_extensions = GetRequiredDeviceExtensions();

  for (const auto &physical_device : physical_devices) {
    auto b = CheckPhysicalDevice(physical_device);

    if (b == true) {
      GetPhysicalDeviceQueueIndices(physical_device);
      return physical_device;
    }
  }

  return nullptr;
}

} // namespace Innsmouth