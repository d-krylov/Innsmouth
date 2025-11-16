#include <GLFW/glfw3.h>
#include "graphics_context.h"
#include "graphics_tools.h"
#include <print>
#include <vector>

namespace Innsmouth {

GraphicsContext *GraphicsContext::graphics_context_instance_ = nullptr;

GraphicsContext *GraphicsContext::Get() {
  return graphics_context_instance_;
}

const VkInstance GraphicsContext::GetInstance() const {
  return instance_;
}

const VkPhysicalDevice GraphicsContext::GetPhysicalDevice() const {
  return physical_device_;
}

const VkDevice GraphicsContext::GetDevice() const {
  return device_;
}

const VkQueue GraphicsContext::GetGeneralQueue() const {
  return general_queue_;
}

const VkCommandPool GraphicsContext::GetGeneralCommandPool() const {
  return general_command_pool_;
}

GraphicsContext::GraphicsContext() {
  CreateInstance();
  PickPhysicalDevice();
  CreateDevice();
  CreateCommandPool();
  graphics_context_instance_ = this;
}

GraphicsContext::~GraphicsContext() {
}

std::vector<const char *> GraphicsContext::GetInstanceLayers() const {
  auto layers = Enumerate(vkEnumerateInstanceLayerProperties);

  std::vector required_layers = {"VK_LAYER_KHRONOS_validation"};

  return required_layers;
}

// clang-format off
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                    VkDebugUtilsMessageTypeFlagsEXT type,
                                                    const VkDebugUtilsMessengerCallbackDataEXT *data,
                                                    void *user_data) {
  std::println("Validation Layer: {0}", data->pMessage);
  return VK_FALSE;
}
// clang-format on

std::vector<const char *> GetSwapchainExtensions() {
  uint32_t extensions_count = 0;
  auto status = glfwInit();
  auto extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
  return std::vector(extensions, extensions + extensions_count);
}

void GraphicsContext::CreateInstance() {
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

  std::vector<const char *> required_layers = GetInstanceLayers();
  std::vector<const char *> required_extensions{VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

  auto swapchain_extensions = GetSwapchainExtensions();
  required_extensions.insert(required_extensions.end(), swapchain_extensions.begin(), swapchain_extensions.end());

  VkDebugUtilsMessengerCreateInfoEXT debug_ci{};
  {
    debug_ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_ci.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    debug_ci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    debug_ci.pfnUserCallback = DebugCallback;
  }

  VkInstanceCreateInfo instance_ci{};
  {
    instance_ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_ci.pApplicationInfo = &application_info;
    instance_ci.enabledLayerCount = required_layers.size();
    instance_ci.ppEnabledLayerNames = required_layers.data();
    instance_ci.enabledExtensionCount = required_extensions.size();
    instance_ci.ppEnabledExtensionNames = required_extensions.data();
  }

  instance_ci.pNext = &debug_ci;

  VK_CHECK(vkCreateInstance(&instance_ci, nullptr, &instance_));

  volkLoadInstanceOnly(instance_);

  VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance_, &debug_ci, nullptr, &debug_messenger_));
}

void GraphicsContext::CreateCommandPool() {
  VkCommandPoolCreateInfo command_pool_ci{};
  {
    command_pool_ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_ci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    command_pool_ci.queueFamilyIndex = general_queue_index_;
  }
  VK_CHECK(vkCreateCommandPool(device_, &command_pool_ci, nullptr, &general_command_pool_));
}

void GraphicsContext::PickPhysicalDevice() {
  auto physical_devices = Enumerate(vkEnumeratePhysicalDevices, instance_);

  for (const auto &physical_device : physical_devices) {
    auto b = EvaluatePhysicalDevice(physical_device);

    if (b == true) {
      physical_device_ = physical_device;
      break;
    }
  }
}

void GraphicsContext::CreateDevice() {
  auto queue_indices = PickPhysicalDeviceQueues(physical_device_);

  general_queue_index_ = queue_indices.general;
  compute_queue_index_ = queue_indices.compute;
  transfer_queue_index_ = queue_indices.transfer;

  std::array queue_priorities = {0.0f};

  std::vector<VkDeviceQueueCreateInfo> device_queue_cis;

  for (const auto &queue_index : {general_queue_index_, compute_queue_index_, transfer_queue_index_}) {

    if (queue_index != -1) {

      VkDeviceQueueCreateInfo queue_ci{};
      {
        queue_ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_ci.queueFamilyIndex = queue_index;
        queue_ci.pQueuePriorities = queue_priorities.data();
        queue_ci.queueCount = 1;
      }

      device_queue_cis.emplace_back(queue_ci);
    }
  }

  auto required_device_extensions = GetRequiredDeviceExtensions();

  VkPhysicalDeviceVulkan14Features physical_device_features_14{};
  {
    physical_device_features_14.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
    physical_device_features_14.maintenance5 = true;
    physical_device_features_14.maintenance6 = true;
    physical_device_features_14.pushDescriptor = true;
    physical_device_features_14.pNext = nullptr;
  }

  VkPhysicalDeviceVulkan13Features physical_device_features_13{};
  {
    physical_device_features_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    physical_device_features_13.synchronization2 = VK_TRUE;
    physical_device_features_13.dynamicRendering = VK_TRUE;
    physical_device_features_13.pNext = &physical_device_features_14;
  }

  VkPhysicalDeviceFeatures2 physical_device_features_2{};
  {
    physical_device_features_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    physical_device_features_2.pNext = &physical_device_features_13;
  }

  VkDeviceCreateInfo device_ci{};
  {
    device_ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_ci.pQueueCreateInfos = device_queue_cis.data();
    device_ci.queueCreateInfoCount = device_queue_cis.size();
    device_ci.ppEnabledExtensionNames = required_device_extensions.data();
    device_ci.enabledExtensionCount = required_device_extensions.size();
    device_ci.pNext = &physical_device_features_2;
  }

  VK_CHECK(vkCreateDevice(physical_device_, &device_ci, nullptr, &device_));

  volkLoadDevice(device_);

  vkGetDeviceQueue(device_, general_queue_index_, 0, &general_queue_);
  vkGetDeviceQueue(device_, compute_queue_index_, 0, &compute_queue_);
  vkGetDeviceQueue(device_, transfer_queue_index_, 0, &transfer_queue_);
}

} // namespace Innsmouth