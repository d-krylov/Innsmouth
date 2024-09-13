#include "depth_image.h"
#include "innsmouth/graphics/include/graphics.h"
#include "innsmouth/graphics/include/graphics_helpers.h"

namespace Innsmouth {

DepthImage::DepthImage(uint32_t width, uint32_t height, Format format, SampleCount samples)
  : Image(width, height, 1, format, 1, 1, samples, Filter::LINEAR, Filter::LINEAR,
          SamplerAddressMode::CLAMP_TO_EDGE) {

  CORE_VERIFY(format_ != Format::UNDEFINED);

  VkImageAspectFlags aspect = VK_IMAGE_ASPECT_DEPTH_BIT;

  aspect |= vkuFormatHasStencil(VkFormat(format_)) ? VK_IMAGE_ASPECT_STENCIL_BIT : 0;

  CreateImage(image_, allocation_, ImageType::_2D, extent_, levels_, layers_, format_, ImageTiling::OPTIMAL,
              ImageUsage::SAMPLED | ImageUsage::DEPTH_STENCIL_ATTACHMENT);

  CreateImageView(image_, image_view_, format_, ImageViewType::_2D,
                  CreateImageSubresourceRange(ImageAspect::DEPTH));

  CreateImageSampler(sampler_, min_, mag_, sampler_address_);
}

} // namespace Innsmouth