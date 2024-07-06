#ifndef INNSMOUTH_IMAGE_2D_H
#define INNSMOUTH_IMAGE_2D_H

#include "core/include/core_image.h"
#include "graphics/image/image.h"
#include <span>

namespace Innsmouth {

class Image2D : public Image {
public:
  Image2D(const CoreImage &core_image);

  Image2D(const VkExtent2D &extent, std::span<std::byte> data);

  void SetData(std::span<std::byte> data);

private:
  uint32_t channels_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_2D_H