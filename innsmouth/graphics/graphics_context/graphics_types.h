#ifndef INNSMOUTH_GRAPHICS_TYPES_H
#define INNSMOUTH_GRAPHICS_TYPES_H

#include <volk/volk.h>
#include "innsmouth/graphics/core/graphics_enums.h"

namespace Innsmouth {

AccessMask2 GetAccessMask(ImageLayout layout);

bool HasBits(VkFlags supported_mask, VkFlags requiered_mask);
bool NotBits(VkFlags supported_mask, VkFlags requiered_mask);

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_TYPES_H