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

const VkQueue GraphicsContext::GetGraphicsQueue() const {
  return graphics_queue_;
}

uint32_t GraphicsContext::GetGraphicsQueueIndex() const {
  return graphics_queue_index_;
}

GraphicsContext::GraphicsContext() {
  CreateInstance();
  PickPhysicalDevice();
  CreateDevice();
  graphics_context_instance_ = this;
}

GraphicsContext::~GraphicsContext() {
}

std::vector<const char *> GraphicsContext::GetInstanceLayers() const {
  auto layers = Enumerate<LayerProperties>(vkEnumerateInstanceLayerProperties);

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

  ApplicationInfo application_info{};
  application_info.pApplicationName = "Innsmouth Application";
  application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  application_info.pEngineName = "Innsmouth Engine";
  application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  application_info.apiVersion = VK_API_VERSION_1_3;

  std::vector<const char *> required_layers = GetInstanceLayers();
  std::vector<const char *> required_extensions{VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

  auto swapchain_extensions = GetSwapchainExtensions();
  required_extensions.insert(required_extensions.end(), swapchain_extensions.begin(), swapchain_extensions.end());

  std::array enabled_validation{ValidationFeatureEnableEXT::E_SYNCHRONIZATION_VALIDATION_EXT, ValidationFeatureEnableEXT::E_BEST_PRACTICES_EXT};

  ValidationFeaturesEXT validation_features;
  validation_features.enabledValidationFeatureCount = enabled_validation.size();
  validation_features.pEnabledValidationFeatures = enabled_validation.data();

  DebugUtilsMessengerCreateInfoEXT debug_ci{};
  debug_ci.messageType = DebugUtilsMessageTypeMaskBitsEXT::E_VALIDATION_BIT_EXT | DebugUtilsMessageTypeMaskBitsEXT::E_GENERAL_BIT_EXT;
  debug_ci.messageSeverity = DebugUtilsMessageSeverityMaskBitsEXT::E_ERROR_BIT_EXT | DebugUtilsMessageSeverityMaskBitsEXT::E_INFO_BIT_EXT |
                             DebugUtilsMessageSeverityMaskBitsEXT::E_WARNING_BIT_EXT | DebugUtilsMessageSeverityMaskBitsEXT::E_VERBOSE_BIT_EXT;

  debug_ci.pfnUserCallback = DebugCallback;
  debug_ci.pNext = &validation_features;

  InstanceCreateInfo instance_ci{};
  instance_ci.pApplicationInfo = &application_info;
  instance_ci.enabledLayerCount = required_layers.size();
  instance_ci.ppEnabledLayerNames = required_layers.data();
  instance_ci.enabledExtensionCount = required_extensions.size();
  instance_ci.ppEnabledExtensionNames = required_extensions.data();

  instance_ci.pNext = &debug_ci;

  VK_CHECK(vkCreateInstance(instance_ci, nullptr, &instance_));

  volkLoadInstanceOnly(instance_);

  VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance_, debug_ci, nullptr, &debug_messenger_));
}

void GraphicsContext::PickPhysicalDevice() {
  auto physical_devices = Enumerate<VkPhysicalDevice>(vkEnumeratePhysicalDevices, instance_);

  for (const auto &physical_device : physical_devices) {
    auto b = EvaluatePhysicalDevice(physical_device);

    if (b == true) {
      physical_device_ = physical_device;
      break;
    }
  }
}

void GraphicsContext::CreateDevice() {
  graphics_queue_index_ = PickPhysicalDeviceQueue(physical_device_);

  std::array queue_priorities = {0.0f};

  std::array<DeviceQueueCreateInfo, 1> device_queue_cis;

  device_queue_cis[0].queueFamilyIndex = graphics_queue_index_;
  device_queue_cis[0].pQueuePriorities = queue_priorities.data();
  device_queue_cis[0].queueCount = 1;

  auto required_device_extensions = GetRequiredDeviceExtensions();

  PhysicalDeviceRayTracingPipelineFeaturesKHR physical_device_ray_tracing_pipeline_features;
  physical_device_ray_tracing_pipeline_features.rayTracingPipeline = true;
  physical_device_ray_tracing_pipeline_features.rayTracingPipelineTraceRaysIndirect = true;
  physical_device_ray_tracing_pipeline_features.pNext = nullptr;

  PhysicalDeviceRayQueryFeaturesKHR physical_device_ray_query_features{};
  physical_device_ray_query_features.rayQuery = true;
  physical_device_ray_query_features.pNext = &physical_device_ray_tracing_pipeline_features;

  PhysicalDeviceAccelerationStructureFeaturesKHR physical_device_acceleration_structure_features;
  physical_device_acceleration_structure_features.accelerationStructure = true;
  physical_device_acceleration_structure_features.pNext = &physical_device_ray_query_features;

  PhysicalDeviceVulkan14Features physical_device_features_14;
  physical_device_features_14.maintenance5 = true;
  physical_device_features_14.maintenance6 = true;
  physical_device_features_14.pushDescriptor = true;
  physical_device_features_14.pNext = &physical_device_acceleration_structure_features;

  PhysicalDeviceVulkan13Features physical_device_features_13;
  physical_device_features_13.synchronization2 = true;
  physical_device_features_13.dynamicRendering = true;
  physical_device_features_13.pNext = &physical_device_features_14;

  PhysicalDeviceVulkan12Features physical_device_features_12{};
  physical_device_features_12.bufferDeviceAddress = true;
  physical_device_features_12.descriptorIndexing = true;
  physical_device_features_12.shaderSampledImageArrayNonUniformIndexing = true;
  physical_device_features_12.descriptorBindingSampledImageUpdateAfterBind = true;
  physical_device_features_12.descriptorBindingUpdateUnusedWhilePending = true;
  physical_device_features_12.descriptorBindingPartiallyBound = true;
  physical_device_features_12.descriptorBindingVariableDescriptorCount = true;
  physical_device_features_12.runtimeDescriptorArray = true;
  physical_device_features_12.drawIndirectCount = true;
  physical_device_features_12.pNext = &physical_device_features_13;

  PhysicalDeviceVulkan11Features physical_device_features_11;
  physical_device_features_11.shaderDrawParameters = true;
  physical_device_features_11.pNext = &physical_device_features_12;

  PhysicalDeviceFeatures2 physical_device_features_2;
  physical_device_features_2.pNext = &physical_device_features_11;
  physical_device_features_2.features.multiDrawIndirect = true;

  DeviceCreateInfo device_ci{};
  device_ci.pQueueCreateInfos = device_queue_cis.data();
  device_ci.queueCreateInfoCount = device_queue_cis.size();
  device_ci.ppEnabledExtensionNames = required_device_extensions.data();
  device_ci.enabledExtensionCount = required_device_extensions.size();
  device_ci.pNext = &physical_device_features_2;

  VK_CHECK(vkCreateDevice(physical_device_, device_ci, nullptr, &device_));

  volkLoadDevice(device_);

  vkGetDeviceQueue(device_, graphics_queue_index_, 0, &graphics_queue_);
}

} // namespace Innsmouth