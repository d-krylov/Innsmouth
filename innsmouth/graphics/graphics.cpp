#include "graphics/include/graphics.h"
#include "graphics/include/graphics_tools.h"
#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"
// #define NDEBUG
#include <cassert>
#include <print>

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
VkCommandPool graphics_command_pool_{VK_NULL_HANDLE};
VkCommandPool compute_command_pool_{VK_NULL_HANDLE};
VkCommandPool transfer_command_pool_{VK_NULL_HANDLE};
VmaAllocator vma_allocator_{VK_NULL_HANDLE};

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

const VkQueue TransferQueue() {
  assert(transfer_queue_ != VK_NULL_HANDLE);
  return transfer_queue_;
}

const VkQueue ComputeQueue() {
  assert(compute_queue_ != VK_NULL_HANDLE);
  return compute_queue_;
}

const VkCommandPool GraphicsCommandPool() {
  assert(graphics_command_pool_ != VK_NULL_HANDLE);
  return graphics_command_pool_;
}

const VkCommandPool TransferCommandPool() {
  assert(transfer_command_pool_ != VK_NULL_HANDLE);
  return transfer_command_pool_;
}

const VkCommandPool ComputeCommandPool() {
  assert(compute_command_pool_ != VK_NULL_HANDLE);
  return compute_command_pool_;
}

const VmaAllocator Allocator() {
  assert(vma_allocator_ != VK_NULL_HANDLE);
  return vma_allocator_;
}

void WaitIdle() {
  assert(device_ != VK_NULL_HANDLE);
  vkDeviceWaitIdle(device_);
}

// clang-format off
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                    VkDebugUtilsMessageTypeFlagsEXT type,
                                                    const VkDebugUtilsMessengerCallbackDataEXT *data,
                                                    void *user_data) {
  std::print("{0} {1}\n", "Validation Layer:", data->pMessage);
  return VK_FALSE;
}
// clang-format on

// INSTANCE
void CreateInstance(const std::vector<const char *> &instance_extensions, const std::optional<DebugOptions> debug_options = std::nullopt) {

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

  required_extensions.insert(required_extensions.end(), instance_extensions.begin(), instance_extensions.end());

  VkDebugUtilsMessengerCreateInfoEXT debug_ci{};

  auto debug = debug_options.and_then([&](const DebugOptions &options) {
    required_layers.emplace_back("VK_LAYER_KHRONOS_validation");
    required_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    debug_ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_ci.messageType = VkDebugUtilsMessageTypeFlagsEXT(options.type);
    debug_ci.messageSeverity = VkDebugUtilsMessageSeverityFlagsEXT(options.severity);
    debug_ci.pfnUserCallback = DebugCallback;
    return std::make_optional(&debug_ci);
  });

  VkInstanceCreateInfo instance_ci{};
  {
    instance_ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_ci.pApplicationInfo = &application_info;
    instance_ci.enabledLayerCount = required_layers.size();
    instance_ci.ppEnabledLayerNames = required_layers.data();
    instance_ci.enabledExtensionCount = required_extensions.size();
    instance_ci.ppEnabledExtensionNames = required_extensions.data();
    instance_ci.pNext = debug.value_or(nullptr);
  }

  VK_CHECK(vkCreateInstance(&instance_ci, nullptr, &instance_));

  volkLoadInstanceOnly(instance_);

  if (debug.has_value()) {
    VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance_, debug.value(), nullptr, &debug_messenger_));
  }
}

// DEVICE
void CreateDevice() {

  auto required_device_extensions = GetRequiredDeviceExtensions();

  DeviceFeatures device_features(GetRequredDeviceFeatures(), GetRequredDeviceFeatures11(), GetRequredDeviceFeatures12(),
                                 GetRequredDeviceFeatures13());

  float queue_priorities[1] = {0.0f};

  std::vector<VkDeviceQueueCreateInfo> device_queue_ci;

  for (auto &index : {graphics_queue_index_, compute_queue_index_, transfer_queue_index_}) {
    if (index != -1) {
      VkDeviceQueueCreateInfo queue_ci{};
      {
        queue_ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_ci.queueFamilyIndex = index;
        queue_ci.pQueuePriorities = queue_priorities;
        queue_ci.queueCount = 1;
      }
      device_queue_ci.emplace_back(queue_ci);
    }
  }

  VkDeviceCreateInfo device_ci{};
  {
    device_ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_ci.pQueueCreateInfos = device_queue_ci.data();
    device_ci.queueCreateInfoCount = device_queue_ci.size();
    device_ci.ppEnabledExtensionNames = required_device_extensions.data();
    device_ci.enabledExtensionCount = required_device_extensions.size();
    device_ci.pNext = &device_features.features_;
  }

  VK_CHECK(vkCreateDevice(PhysicalDevice(), &device_ci, nullptr, &device_));

  volkLoadDevice(device_);

  vkGetDeviceQueue(device_, graphics_queue_index_, 0, &graphics_queue_);
  vkGetDeviceQueue(device_, compute_queue_index_, 0, &compute_queue_);
  vkGetDeviceQueue(device_, transfer_queue_index_, 0, &transfer_queue_);
}

// COMMAND POOL
void CreateCommandPool(uint32_t queue_index, VkCommandPool &command_pool) {
  VkCommandPoolCreateInfo command_pool_ci{};
  {
    command_pool_ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_ci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    command_pool_ci.queueFamilyIndex = queue_index;
  }
  VK_CHECK(vkCreateCommandPool(Device(), &command_pool_ci, nullptr, &command_pool));
}

// ALLOCATOR
void CreateAllocator() {
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
    // allocator_ci.flags = ;
  }

  VK_CHECK(vmaCreateAllocator(&allocator_ci, &vma_allocator_));
}

Graphics::Graphics(const std::vector<const char *> &extensions, const std::optional<DebugOptions> &debug_options) {
  CreateInstance(extensions, debug_options);

  auto v = Enumerate(vkEnumeratePhysicalDevices, instance_);

  physical_device_ = PickPhysicalDevice(v);

  auto queue_indices = GetPhysicalDeviceQueueIndices(physical_device_);

  graphics_queue_index_ = queue_indices.graphics_.value_or(-1);
  transfer_queue_index_ = queue_indices.transfer_.value_or(-1);
  compute_queue_index_ = queue_indices.compute_.value_or(-1);

  CreateDevice();

  CreateCommandPool(graphics_queue_index_, graphics_command_pool_);

  CreateAllocator();
}

Graphics::~Graphics() {}

} // namespace Innsmouth