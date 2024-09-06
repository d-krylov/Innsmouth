#ifndef INNSMOUTH_IMAGE_2D_H
#define INNSMOUTH_IMAGE_2D_H

#include "innsmouth/core/include/core_image.h"
#include "innsmouth/graphics/image/image.h"
#include <span>

namespace Innsmouth {

class Image2D : public Image {
public:
  Image2D() = default;

  Image2D(const CoreImage &core_image);

  Image2D(const VkExtent2D &extent, std::span<const std::byte> data);

  void SetData(std::span<const std::byte> data, std::size_t buffer_size = 0);

private:
  uint32_t channels_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_2D_H