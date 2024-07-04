#ifndef INNSMOUTH_IMAGE_2D_H
#define INNSMOUTH_IMAGE_2D_H

#include "graphics/image/image.h"
#include <span>

namespace Innsmouth {

class Image2D : public Image {
public:
  Image2D();

  void SetData(std::span<std::byte> data, uint32_t layers, uint32_t base_layer);

private:
  uint32_t channels_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_2D_H