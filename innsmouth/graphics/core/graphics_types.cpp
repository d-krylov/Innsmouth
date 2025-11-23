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

ImageAspectMask GetAspectMask(Format format) {
  switch (format) {
  case Format::E_D16_UNORM:
  case Format::E_X8_D24_UNORM_PACK32:
  case Format::E_D32_SFLOAT: return ImageAspectMaskBits::E_DEPTH_BIT;
  case Format::E_S8_UINT: return ImageAspectMaskBits::E_STENCIL_BIT;
  case Format::E_D16_UNORM_S8_UINT:
  case Format::E_D24_UNORM_S8_UINT:
  case Format::E_D32_SFLOAT_S8_UINT: return ImageAspectMaskBits::E_DEPTH_BIT | ImageAspectMaskBits::E_STENCIL_BIT;
  default: break;
  }
  return ImageAspectMaskBits::E_COLOR_BIT;
}

bool HasBits(VkFlags supported_mask, VkFlags requiered_mask) {
  return (supported_mask & requiered_mask) == requiered_mask;
}

bool NotBits(VkFlags supported_mask, VkFlags requiered_mask) {
  return (supported_mask & requiered_mask) == 0;
}

} // namespace Innsmouth