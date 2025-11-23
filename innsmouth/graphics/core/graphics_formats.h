#ifndef INNSMOUTH_FORMATS_H
#define INNSMOUTH_FORMATS_H

#include "graphics_structures.h"
#include <array>

namespace Innsmouth {

inline constexpr auto MAX_COMPONENTS = 4;

struct FormatComponentInformation {
  char name = 0;
  uint32_t bits = 0;
};

struct FormatInformation {
  uint32_t texel_block_size = 0;
  uint32_t texels_per_block = 0;
  Extent3D extent = {0, 0, 0};
  uint32_t component_count = 0;
  FormatComponentInformation components[MAX_COMPONENTS];
};

const FormatInformation &GetFormatInformation(Format format);

uint32_t GetFormatTexelBlockSize(Format format);

} // namespace Innsmouth

#endif // INNSMOUTH_FORMATS_H