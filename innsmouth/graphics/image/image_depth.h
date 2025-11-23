#ifndef INNSMOUTH_IMAGE_DEPTH_H
#define INNSMOUTH_IMAGE_DEPTH_H

#include "image.h"

namespace Innsmouth {

class ImageDepth : public Image {
public:
  ImageDepth() = default;

  ImageDepth(uint32_t width, uint32_t height, Format format = Format::E_D32_SFLOAT);

  ImageDepth(ImageDepth &&other) noexcept = default;

  ImageDepth &operator=(ImageDepth &&other) noexcept = default;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_DEPTH_H