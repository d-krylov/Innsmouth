#include "graphics_tools.h"
#include <print>
#include <ranges>
#include <vulkan/vk_enum_string_helper.h>

namespace Innsmouth {

void VK_CHECK(VkResult result, std::source_location source_location) {
  if (result != VK_SUCCESS) {
    std::println("VULKAN ERROR: {0} in {1}:{2}", string_VkResult(result), source_location.file_name(), source_location.line());
  }
}

std::vector<const char *> GetRequiredDeviceExtensions() {
  return {
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,        //
    VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,          //
    VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,   //
    VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME, //
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,        //
    VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,   //
    VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME, //
    VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,     //
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,      //
    VK_KHR_RAY_QUERY_EXTENSION_NAME,                //
    VK_KHR_SWAPCHAIN_EXTENSION_NAME                 //
  };
}

bool EvaluatePhysicalDevice(const VkPhysicalDevice physical_device) {

  VkPhysicalDeviceProperties device_properties{};
  vkGetPhysicalDeviceProperties(physical_device, &device_properties);

  bool b = true;
  b &= (device_properties.apiVersion >= VK_API_VERSION_1_3);
  b &= (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);

  return b;
}

int32_t PickPhysicalDeviceQueue(const VkPhysicalDevice physical_device) {
  int32_t graphics_queue_index{-1};
  auto queue_properties = Enumerate<QueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, physical_device);
  for (const auto &[queue_index, queue_property] : std::views::enumerate(queue_properties)) {
    QueueMask queue_mask(queue_property.queueFlags);
    if (queue_mask.HasBits(QueueMaskBits::E_GRAPHICS_BIT | QueueMaskBits::E_COMPUTE_BIT | QueueMaskBits::E_TRANSFER_BIT)) {
      graphics_queue_index = queue_index;
    }
  }
  return graphics_queue_index;
}

} // namespace Innsmouth