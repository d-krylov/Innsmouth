#include "graphics/include/graphics_tools.h"
#include "graphics/include/graphics.h"
#include <bit>

namespace Innsmouth {

// FEATURES AND EXTENSIONS
VkPhysicalDeviceFeatures2 GetRequiredFeatures() {
  VkPhysicalDeviceFeatures2 required{};
  {
    required.features.geometryShader = VK_TRUE;
    required.features.tessellationShader = VK_TRUE;
    required.features.multiDrawIndirect = VK_TRUE;
  }
  return required;
}

VkPhysicalDeviceVulkan11Features GetRequiredFeatures11() {
  VkPhysicalDeviceVulkan11Features required{};
  required.shaderDrawParameters = VK_TRUE;
  return required;
}

VkPhysicalDeviceVulkan12Features GetRequiredFeatures12() {
  VkPhysicalDeviceVulkan12Features required{};
  required.descriptorIndexing = VK_TRUE;
  return required;
}

VkPhysicalDeviceVulkan13Features GetRequiredFeatures13() {
  VkPhysicalDeviceVulkan13Features required{};
  required.dynamicRendering = VK_TRUE;
  return required;
}

VkPhysicalDeviceExtendedDynamicStateFeaturesEXT GetExtendedDynamicState() {
  VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extended_dynamic_state{};
  extended_dynamic_state.extendedDynamicState = VK_TRUE;
  return extended_dynamic_state;
}

std::vector<const char *> GetRequiredDeviceExtensions() {
  return std::vector{VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
                     VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
                     VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
                     VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
                     VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME,
                     VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
                     VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

// SELECT PHYSICAL DEVICE

std::optional<uint32_t> GetQueueFamilyIndex(std::span<const VkQueueFamilyProperties> p, QueueFlags flags) {
  auto required = VkQueueFlags(flags);
  auto predicate = [&](const VkQueueFamilyProperties &p) { return (p.queueFlags & required) == required; };
  auto it = std::find_if(p.begin(), p.end(), predicate);
  return (it != p.end()) ? std::make_optional(std::distance(p.begin(), it)) : std::nullopt;
}

bool CheckSupportedExtensions(const std::vector<VkExtensionProperties> &supported,
                              const std::vector<const char *> &required) {
  for (const auto &required_extension : required) {
    auto predicate = [required_extension](const VkExtensionProperties &supported_extension) {
      return std::string_view(supported_extension.extensionName) == required_extension;
    };

    if (std::none_of(supported.begin(), supported.end(), predicate)) {
      return false;
    }
  }
  return true;
}

bool CheckFeatures(const DeviceFeatures &device_features) {

  auto features = GetRequiredFeatures();
  auto required = features.features;
  auto supported = device_features.features_.features;

  constexpr auto n = sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32);
  auto L = std::bit_cast<std::array<VkBool32, n>>(supported);
  auto R = std::bit_cast<std::array<VkBool32, n>>(required);

  auto b = std::equal(R.begin(), R.end(), L.begin(), [](auto r, auto s) { return !r || s; });

  b &= device_features.features_1_3_.dynamicRendering;
  b &= device_features.dynamic_state_features_.extendedDynamicState;
  b &= device_features.features_1_2_.descriptorIndexing;

  return b;
}

bool CheckPhysicalDeviceFeaturesAndProperties(const VkPhysicalDevice physical_device) {
  VkPhysicalDeviceProperties2 properties;
  VkPhysicalDeviceVulkan11Properties properties_1_1;

  DeviceFeatures device_features;

  properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
  properties.pNext = nullptr;

  vkGetPhysicalDeviceProperties2(physical_device, &properties);

  vkGetPhysicalDeviceFeatures2(physical_device, &device_features.features_);

  if (properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
    return false;
  }

  if (properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
    return false;
  }

  if (properties.properties.apiVersion < VK_API_VERSION_1_3) {
    return false;
  }

  auto b = CheckFeatures(device_features);

  if (b == false) {
    return false;
  }

  return properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

VkPhysicalDevice PickPhysicalDevice(std::span<const VkPhysicalDevice> physical_devices) {
  VkPhysicalDevice ret{VK_NULL_HANDLE};

  auto required_extensions = GetRequiredDeviceExtensions();

  for (const auto physical_device : physical_devices) {

    bool status = CheckPhysicalDeviceFeaturesAndProperties(physical_device);

    if (status != true) {
      continue;
    }

    auto p = Enumerate<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, physical_device);

    auto graphics_family_index = GetQueueFamilyIndex(p, QueueFlags::GRAPHICS);

    if (graphics_family_index.has_value() == false) {
      continue;
    }

    auto supported_extensions =
      Enumerate<VkExtensionProperties>(vkEnumerateDeviceExtensionProperties, physical_device, nullptr);

    status = CheckSupportedExtensions(supported_extensions, required_extensions);

    if (status == false) {
      continue;
    }

    if (ret == VK_NULL_HANDLE && status == true) {
      ret = physical_device;
    }
  }

  return ret;
}

} // namespace Innsmouth