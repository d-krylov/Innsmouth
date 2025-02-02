#include "graphics/include/graphics_types.h"
#include <vulkan/vk_enum_string_helper.h>
#include <print>

namespace Innsmouth {

void VK_CHECK(VkResult result, std::source_location source_location) {
  if (result != VK_SUCCESS) {
    std::println("VULKAN ERROR: {0}", string_VkResult(result));
  }
}

} // namespace Innsmouth