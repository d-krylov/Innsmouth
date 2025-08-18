#include "graphics_types.h"

namespace Innsmouth {

VkAccessFlags GetAccessMask(VkImageLayout layout) {
  switch (layout) {
  case VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED:
  case VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
    return 0;
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

bool HasBits(VkFlags supported_mask, VkFlags requiered_mask) {
  return (supported_mask & requiered_mask) == requiered_mask;
}

bool NotBits(VkFlags supported_mask, VkFlags requiered_mask) {
  return (supported_mask & requiered_mask) == 0;
}

} // namespace Innsmouth