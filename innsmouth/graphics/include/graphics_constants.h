#ifndef INNSMOUTH_GRAPHICS_CONSTANTS_H
#define INNSMOUTH_GRAPHICS_CONSTANTS_H

#include "graphics_types.h"

namespace Innsmouth {

[[nodiscard]] std::vector<const char *> GetDeviceExtensions();
[[nodiscard]] std::vector<Format> GetDepthFormats();

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_CONSTANTS_H