#include "graphics/include/graphics_types.h"
#include "easyloggingpp/easylogging++.h"
#include <cmath>
#include <unordered_map>

namespace Innsmouth {

void VK_CHECK(VkResult result) {
  if (result != VK_SUCCESS) {
    LOG(FATAL) << string_VkResult(result);
  }
}

// clang-format off
VkAccessFlags GetAccessMask(VkImageLayout layout) {
  switch (layout) {
  case VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED: return 0;
  case VkImageLayout::VK_IMAGE_LAYOUT_PREINITIALIZED: 
    return VkAccessFlagBits::VK_ACCESS_HOST_WRITE_BIT;
  case VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: 
    return VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  case VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: 
    return VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  case VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: 
    return VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT;
  case VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: 
    return VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT;
  case VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: 
    return VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
  default:
    break;
  }
}
// clang-format on

} // namespace Innsmouth