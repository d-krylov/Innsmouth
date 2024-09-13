#include "innsmouth/graphics/include/graphics_constants.h"

namespace Innsmouth {

std::vector<const char *> GetDeviceExtensions() {
  return std::vector<const char *>{
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
    VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME, VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME};
}

std::vector<Format> GetDepthFormats() {
  return std::vector<Format>{Format::D32_SFLOAT_S8_UINT, Format::D32_SFLOAT, Format::D24_UNORM_S8_UINT,
                             Format::D16_UNORM_S8_UINT, Format::D16_UNORM};
}

} // namespace Innsmouth