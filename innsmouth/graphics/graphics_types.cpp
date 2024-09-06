#include "innsmouth/graphics/include/graphics_types.h"
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

uint32_t GetMipLevels(const VkExtent3D &e) {
  auto m = std::max(e.width, std::max(e.height, e.depth));
  return static_cast<uint32_t>(std::floor(std::log2(m))) + 1;
}

// clang-format off
VkImageSubresourceRange CreateImageSubresourceRange(ImageAspect aspect, uint32_t base_level,
                                                    uint32_t levels, uint32_t base_layer,
                                                    uint32_t layers) {
  return VkImageSubresourceRange{
    .aspectMask = VkImageAspectFlags(aspect),
    .baseMipLevel = base_level,
    .levelCount = levels,
    .baseArrayLayer = base_layer,
    .layerCount = layers
  };
}
// clang-format on

std::vector<VkDynamicState> GetDynamicStates() {
  return std::vector<VkDynamicState>{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR,
                                     VK_DYNAMIC_STATE_CULL_MODE, VK_DYNAMIC_STATE_FRONT_FACE};
}

} // namespace Innsmouth