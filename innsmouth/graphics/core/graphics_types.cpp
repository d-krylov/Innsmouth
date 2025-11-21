#include "graphics_types.h"

namespace Innsmouth {

AccessMask2 GetAccessMask(ImageLayout layout) {
  switch (layout) {
  case ImageLayout::E_UNDEFINED:
  case ImageLayout::E_PRESENT_SRC_KHR: return {};
  case ImageLayout::E_PREINITIALIZED: return AccessMaskBits2::E_HOST_WRITE_BIT;
  case ImageLayout::E_COLOR_ATTACHMENT_OPTIMAL: return AccessMaskBits2::E_COLOR_ATTACHMENT_WRITE_BIT;
  case ImageLayout::E_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: return AccessMaskBits2::E_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  case ImageLayout::E_TRANSFER_SRC_OPTIMAL: return AccessMaskBits2::E_TRANSFER_READ_BIT;
  case ImageLayout::E_TRANSFER_DST_OPTIMAL: return AccessMaskBits2::E_TRANSFER_WRITE_BIT;
  case ImageLayout::E_SHADER_READ_ONLY_OPTIMAL: return AccessMaskBits2::E_SHADER_READ_BIT;
  default: break;
  }
}

bool HasBits(VkFlags supported_mask, VkFlags requiered_mask) {
  return (supported_mask & requiered_mask) == requiered_mask;
}

bool NotBits(VkFlags supported_mask, VkFlags requiered_mask) {
  return (supported_mask & requiered_mask) == 0;
}

} // namespace Innsmouth