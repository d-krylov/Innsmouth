#include "graphics/include/graphics_description.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

namespace Innsmouth {

[[nodiscard]] std::vector<const char *> GetInstanceExtensions() {
  auto status = glfwInit();
  if (status == GLFW_FALSE) {
  }
  uint32_t extensions_count = 0;
  auto extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
  return std::vector<const char *>(extensions, extensions + extensions_count);
}

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

// clang-format off
GraphicsDescription GraphicsDescription::CreateDefault() {
  return GraphicsDescription{
    .message_type_ = DebugMessageType::GENERAL | DebugMessageType::PERFORMANCE | DebugMessageType::VALIDATION,
    .message_severity_ = DebugMessageSeverity::ERROR | DebugMessageSeverity::VERBOSE | DebugMessageSeverity::WARNING,
    .instance_extensions_ = GetInstanceExtensions(),
    .debug_function_ = DebugMessageCallback
  };
}
// clang-format on

} // namespace Innsmouth