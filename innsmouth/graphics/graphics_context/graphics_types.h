#ifndef INNSMOUTH_GRAPHICS_TYPES_H
#define INNSMOUTH_GRAPHICS_TYPES_H

#include <volk/volk.h>

namespace Innsmouth {

VkAccessFlags GetAccessMask(VkImageLayout layout);

bool HasBits(VkFlags supported_mask, VkFlags requiered_mask);
bool NotBits(VkFlags supported_mask, VkFlags requiered_mask);

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_TYPES_H