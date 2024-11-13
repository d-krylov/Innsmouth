#include "graphics/include/graphics.h"
#include "graphics/include/graphics_tools.h"
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
// #define NDEBUG
#include <assert.h>
#include <cstring>

namespace Innsmouth {

VkInstance instance_{VK_NULL_HANDLE};
VkDebugUtilsMessengerEXT debug_messenger_{VK_NULL_HANDLE};
VkPhysicalDevice physical_device_{VK_NULL_HANDLE};
VkDevice device_{VK_NULL_HANDLE};
int32_t graphics_queue_index_{-1};
int32_t transfer_queue_index_{-1};
int32_t compute_queue_index_{-1};
VkQueue graphics_queue_{VK_NULL_HANDLE};
VkQueue transfer_queue_{VK_NULL_HANDLE};
VkQueue compute_queue_{VK_NULL_HANDLE};
VmaAllocator vma_allocator_{VK_NULL_HANDLE};
VkCommandPool command_pool_{VK_NULL_HANDLE};

// GETTERS

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

// clang-format off
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessageCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity, 
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT *data, 
    void *user_data) {
  std::printf("Validation Layer: %s\n", data->pMessage);
  return VK_FALSE;
}
// clang-format on

void CreateInstance(const GraphicsDescription &gd) {
  VK_CHECK(volkInitialize());

  VkApplicationInfo application_info{};
  {
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pApplicationName = "Innsmouth Application";
    application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.pEngineName = "Innsmouth Engine";
    application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.apiVersion = VK_API_VERSION_1_3;
  }

  std::vector<const char *> required_layers;
  std::vector<const char *> required_extensions{VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};

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
    debug_ci.pfnUserCallback = DebugMessageCallback;
    debug_ci.pUserData = nullptr;

    instance_ci.pNext = &debug_ci;
  }

  VK_CHECK(vkCreateInstance(&instance_ci, nullptr, &instance_));

  volkLoadInstanceOnly(instance_);

  if (gd.message_type_.has_value()) {
    VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance_, &debug_ci, nullptr, &debug_messenger_));
  }
}

void GetPhysicalDeviceQueueIndices() {
  auto properties =
    Enumerate<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, physical_device_);

  int32_t graphics = -1, compute = -1, transfer = -1, common = -1;

  for (uint32_t i = 0; i < properties.size(); i++) {
    auto f = properties[i].queueFlags;
    if ((f & VK_QUEUE_TRANSFER_BIT) && (f & VK_QUEUE_GRAPHICS_BIT) && (f & VK_QUEUE_COMPUTE_BIT)) {
      common = i;
    } else if (f & VK_QUEUE_GRAPHICS_BIT) {
      graphics = i;
    } else if ((f & VK_QUEUE_COMPUTE_BIT) && !(f & VK_QUEUE_GRAPHICS_BIT)) {
      compute = i;
    } else if ((f & VK_QUEUE_TRANSFER_BIT) && !(f & VK_QUEUE_GRAPHICS_BIT) && !(f & VK_QUEUE_COMPUTE_BIT)) {
      transfer = i;
    }
  }

  graphics_queue_index_ = (graphics != -1) ? graphics : common;
  compute_queue_index_ = (compute != -1) ? compute : common;
  transfer_queue_index_ = (transfer != -1) ? transfer : common;
}

void CreateDevice() {
  std::vector<VkDeviceQueueCreateInfo> device_queue_ci;

  float queue_priorities[1] = {0.0f};

  VkDeviceQueueCreateInfo graphics_queue_ci{};
  {
    graphics_queue_ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphics_queue_ci.queueFamilyIndex = graphics_queue_index_;
    graphics_queue_ci.pQueuePriorities = queue_priorities;
    graphics_queue_ci.queueCount = 1;
  }

  device_queue_ci.emplace_back(graphics_queue_ci);

  if (compute_queue_index_ != -1 && compute_queue_index_ != graphics_queue_index_) {
    VkDeviceQueueCreateInfo compute_queue_ci{};
    {
      compute_queue_ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      compute_queue_ci.queueFamilyIndex = compute_queue_index_;
      compute_queue_ci.pQueuePriorities = queue_priorities;
      compute_queue_ci.queueCount = 1;
    }
    device_queue_ci.emplace_back(compute_queue_ci);
  }

  if (transfer_queue_index_ != -1 && transfer_queue_index_ != graphics_queue_index_ &&
      transfer_queue_index_ != compute_queue_index_) {
    VkDeviceQueueCreateInfo transfer_queue_ci{};
    {
      transfer_queue_ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      transfer_queue_ci.queueFamilyIndex = transfer_queue_index_;
      transfer_queue_ci.pQueuePriorities = queue_priorities;
      transfer_queue_ci.queueCount = 1;
    }
    device_queue_ci.emplace_back(transfer_queue_ci);
  }

  DeviceFeatures device_features(GetRequiredFeatures(), GetRequiredFeatures11(), GetRequiredFeatures12(),
                                 GetRequiredFeatures13(), GetExtendedDynamicState());

  auto device_extensions = GetRequiredDeviceExtensions();

  VkDeviceCreateInfo device_ci{};
  {
    device_ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_ci.pQueueCreateInfos = device_queue_ci.data();
    device_ci.queueCreateInfoCount = device_queue_ci.size();
    device_ci.ppEnabledExtensionNames = device_extensions.data();
    device_ci.enabledExtensionCount = device_extensions.size();
    device_ci.pNext = &device_features.features_;
  }

  VK_CHECK(vkCreateDevice(PhysicalDevice(), &device_ci, nullptr, &device_));

  volkLoadDevice(device_);

  vkGetDeviceQueue(device_, graphics_queue_index_, 0, &graphics_queue_);
  vkGetDeviceQueue(device_, compute_queue_index_, 0, &compute_queue_);
  vkGetDeviceQueue(device_, transfer_queue_index_, 0, &transfer_queue_);
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
  auto physical_devices = Enumerate<VkPhysicalDevice>(vkEnumeratePhysicalDevices, instance_);
  physical_device_ = PickPhysicalDevice(physical_devices);
  CORE_VERIFY(physical_device_ != VK_NULL_HANDLE);
  GetPhysicalDeviceQueueIndices();
  CreateDevice();
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