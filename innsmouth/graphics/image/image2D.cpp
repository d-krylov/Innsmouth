#include "image2D.h"
#include "graphics/include/graphics.h"
#include "graphics/include/structure_tools.h"

namespace Innsmouth {

Image2D::Image2D(uint32_t width, uint32_t height, ImageUsage usage) {
  usage = usage | ImageUsage::SAMPLED_BIT;
  extent_ = VkExtent3D{width, height, 1};
  auto image_ci = CreateImageCreateInfo(extent_.width, extent_.height, 1, 1, 1, Format::R8G8B8A8_UNORM, ImageType::_2D, usage);
  auto sampler_ci = CreateSamplerCreateInfo(Filter::LINEAR, Filter::LINEAR, SamplerAddressMode::CLAMP_TO_EDGE);

  image_ = CreateImage(vma_allocation_, image_ci);
  image_view_ = CreateImageView(image_, Format::R8G8B8A8_UNORM, ImageViewType::_2D, CreateImageSubresourceRange());

  VK_CHECK(vkCreateSampler(Device(), &sampler_ci, nullptr, &image_sampler_));
}

Image2D::Image2D(uint32_t width, uint32_t height, std::span<const std::byte> data) : Image2D(width, height, ImageUsage::TRANSFER_DST_BIT) {
  SetData(data);
}

} // namespace Innsmouth