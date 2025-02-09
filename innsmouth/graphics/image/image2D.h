#ifndef INNSMOUTH_IMAGE_2D_H
#define INNSMOUTH_IMAGE_2D_H

#include "image.h"
#include "core/include/core_image.h"

namespace Innsmouth {

class Image2D : public Image {
public:
  Image2D(uint32_t width, uint32_t height, ImageUsage usage);

  Image2D(uint32_t width, uint32_t height, std::span<const std::byte> data);

  Image2D(const CoreImage &core_image);
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_2D_H