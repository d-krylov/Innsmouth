#include "image_depth.h"
#include "innsmouth/graphics/core/structure_tools.h"

namespace Innsmouth {

ImageDepth::ImageDepth(uint32_t width, uint32_t height, Format format) {
  ImageSpecification depth_image_specification;
  depth_image_specification.extent_ = Extent3D(width, height, 1);
  depth_image_specification.format_ = format;
  depth_image_specification.usage_ = ImageUsageMaskBits::E_DEPTH_STENCIL_ATTACHMENT_BIT;
  Initialize(ImageType::E_2D, ImageViewType::E_2D, depth_image_specification);
}

} // namespace Innsmouth
