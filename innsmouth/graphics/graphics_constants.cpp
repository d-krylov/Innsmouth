#include "graphics/include/graphics_constants.h"

namespace Innsmouth {

std::vector<Format> GetDepthFormats() {
  return std::vector<Format>{Format::D32_SFLOAT_S8_UINT, Format::D32_SFLOAT, Format::D24_UNORM_S8_UINT,
                             Format::D16_UNORM_S8_UINT, Format::D16_UNORM};
}

} // namespace Innsmouth