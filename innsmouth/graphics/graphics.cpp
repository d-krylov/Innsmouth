#include "innsmouth/graphics/include/graphics.h"
#include "innsmouth/graphics/include/graphics_constants.h"
#include "innsmouth/graphics/include/graphics_helpers.h"
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
//#define NDEBUG
#include <assert.h>
#include <cstring>

namespace Innsmouth {

VkInstance instance_{VK_NULL_HANDLE};
VkDebugUtilsMessengerEXT debug_messenger_{VK_NULL_HANDLE};
VkPhysicalDevice physical_device_{VK_NULL_HANDLE};
VkPhysicalDeviceMemoryProperties memory_properties_;
uint32_t graphics_queue_index_{0};
VkDevice device_{VK_NULL_HANDLE};
VkQueue graphics_queue_{VK_NULL_HANDLE};
VmaAllocator vma_allocator_{VK_NULL_HANDLE};
VkCommandPool command_pool_{VK_NULL_HANDLE};

const VkInstance Instance() {
  assert(instance_ != VK_NULL_HANDLE);
  return instance_;
}

const VkPhysicalDevice PhysicalDevice() {
  assert(physical_device_ != VK_NULL_HANDLE);
  return physical_device_;
}

const VkDevice Device() {
  assert(device_ != VK_NULL_HANDLE);
  return device_;
}

const VkQueue GraphicsQueue() {
  assert(graphics_queue_ != VK_NULL_HANDLE);
  return graphics_queue_;
}

const VmaAllocator Allocator() {
  assert(vma_allocator_ != VK_NULL_HANDLE);
  return vma_allocator_;
}

const VkCommandPool CommandPool() {
  assert(command_pool_ != VK_NULL_HANDLE);
  return command_pool_;
}

void WaitIdle() {
  assert(device_ != VK_NULL_HANDLE);
  vkDeviceWaitIdle(device_);
}

uint32_t GraphicsIndex() { return graphics_queue_index_; }

void CreateInstance(const GraphicsDescription &gd) {
  VK_CHECK(volkInitialize());

  VkApplicationInfo application_info{};
  {
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pApplicationName = gd.application_name_.data();
    application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.pEngineName = gd.engine_name_.data();
    application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.apiVersion = VK_API_VERSION_1_3;
  }

  std::vector<const char *> required_layers;
  std::vector<const char *> required_extensions;

  if (gd.message_type_.has_value()) {
    required_layers.emplace_back("VK_LAYER_KHRONOS_validation");
    required_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  required_extensions.insert(required_extensions.end(), gd.instance_extensions_.begin(),
                             gd.instance_extensions_.end());

  VkInstanceCreateInfo instance_ci{};
  {
    instance_ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_ci.pApplicationInfo = &application_info;
    instance_ci.enabledLayerCount = required_layers.size();
    instance_ci.ppEnabledLayerNames = required_layers.data();
    instance_ci.enabledExtensionCount = required_extensions.size();
    instance_ci.ppEnabledExtensionNames = required_extensions.data();
    instance_ci.pNext = nullptr;
  }

  VkDebugUtilsMessengerCreateInfoEXT debug_ci{};

  if (gd.message_type_.has_value()) {
    debug_ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_ci.messageType = static_cast<VkFlags>(gd.message_type_.value());
    debug_ci.messageSeverity = static_cast<VkFlags>(gd.message_severity_);
    debug_ci.pfnUserCallback = gd.debug_function_;
    debug_ci.pUserData = nullptr;

    instance_ci.pNext = &debug_ci;
  }

  VK_CHECK(vkCreateInstance(&instance_ci, nullptr, &instance_));
  volkLoadInstance(instance_);

  if (gd.message_type_.has_value()) {
    VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance_, &debug_ci, nullptr, &debug_messenger_));
  }
}

[[nodiscard]] std::optional<uint32_t> IsDeviceSuitable(const VkPhysicalDevice device,
                                                       const VkPhysicalDeviceFeatures &required_features,
                                                       const std::vector<const char *> &required_extensions) {
  VkPhysicalDeviceProperties device_properties;
  vkGetPhysicalDeviceProperties(device, &device_properties);
  bool is_discrete = device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
  bool is_supported_extensions = GetSupportedExtensions(device, required_extensions);
  auto graphics_index = GetQueueFamily(device, VK_QUEUE_GRAPHICS_BIT);
  auto is_supported_features = GetSupportedFeatures(device, required_features);
  if (graphics_index.has_value() && is_discrete && is_supported_features && is_supported_extensions) {
    return graphics_index;
  }
  return std::nullopt;
}

void PickPhysicalDevice(const GraphicsDescription &gd) {
  auto devices = Enumerate<VkPhysicalDevice>(vkEnumeratePhysicalDevices, instance_);
  VkPhysicalDevice suitable_device{VK_NULL_HANDLE};
  for (const auto &device : devices) {
    auto queue_index = IsDeviceSuitable(device, gd.device_features_, GetDeviceExtensions());
    if (queue_index.has_value()) {
      physical_device_ = device;
      graphics_queue_index_ = queue_index.value();
      break;
    }
  }
}

void CreateDevice(const GraphicsDescription &gd) {
  float queue_priorities[1] = {0.0f};

  VkDeviceQueueCreateInfo graphics_queue_ci{};
  {
    graphics_queue_ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphics_queue_ci.queueFamilyIndex = graphics_queue_index_;
    graphics_queue_ci.pQueuePriorities = queue_priorities;
    graphics_queue_ci.queueCount = 1;
  }

  VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering{};
  {
    dynamic_rendering.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamic_rendering.dynamicRendering = VK_TRUE;
  }

  auto device_extensions = GetDeviceExtensions();

  VkDeviceCreateInfo device_ci{};
  {
    device_ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_ci.pQueueCreateInfos = &graphics_queue_ci;
    device_ci.queueCreateInfoCount = 1;
    device_ci.ppEnabledExtensionNames = device_extensions.data();
    device_ci.enabledExtensionCount = device_extensions.size();
    device_ci.pEnabledFeatures = &gd.device_features_;
    device_ci.pNext = &dynamic_rendering;
  }

  VK_CHECK(vkCreateDevice(PhysicalDevice(), &device_ci, nullptr, &device_));

  volkLoadDevice(device_);

  vkGetDeviceQueue(device_, graphics_queue_index_, 0, &graphics_queue_);
}

void CreateCommandPool(uint32_t queue_index, VkCommandPool *command_pool) {
  VkCommandPoolCreateInfo command_pool_ci{};
  {
    command_pool_ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_ci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    command_pool_ci.queueFamilyIndex = queue_index;
  }
  VK_CHECK(vkCreateCommandPool(device_, &command_pool_ci, nullptr, command_pool));
}

void CreateVmaAllocator(const GraphicsDescription &gd) {

  VmaVulkanFunctions vulkan_functions{};
  {
    vulkan_functions.vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)vkGetInstanceProcAddr;
    vulkan_functions.vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)vkGetDeviceProcAddr;
  }

  VmaAllocatorCreateInfo allocator_ci{};
  {
    allocator_ci.instance = Instance();
    allocator_ci.physicalDevice = PhysicalDevice();
    allocator_ci.device = Device();
    allocator_ci.vulkanApiVersion = VK_API_VERSION_1_3;
    allocator_ci.pVulkanFunctions = &vulkan_functions;
  }
  VK_CHECK(vmaCreateAllocator(&allocator_ci, &vma_allocator_));
}

Graphics::Graphics(const GraphicsDescription &graphics_description) {
  CreateInstance(graphics_description);
  PickPhysicalDevice(graphics_description);
  CORE_VERIFY(physical_device_ != VK_NULL_HANDLE);
  vkGetPhysicalDeviceMemoryProperties(physical_device_, &memory_properties_);
  CreateDevice(graphics_description);
  CORE_VERIFY(device_ != VK_NULL_HANDLE);
  CreateCommandPool(graphics_queue_index_, &command_pool_);
  CreateVmaAllocator(graphics_description);
}

Graphics::~Graphics() {
  vkDestroyCommandPool(device_, command_pool_, nullptr);
  vmaDestroyAllocator(vma_allocator_);
  vkDestroyDevice(device_, nullptr);
  vkDestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
  vkDestroyInstance(instance_, nullptr);
}

} // namespace Innsmouth