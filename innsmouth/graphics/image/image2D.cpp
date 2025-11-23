#include "image2D.h"
#include "innsmouth/core/include/image_wrapper.h"

namespace Innsmouth {

Image2D::Image2D(const std::filesystem::path &image_path, const std::optional<SamplerSpecification> &sampler_specification) {
  ImageWrapper image_wrapper(image_path);
  ImageSpecification image_specification;
  image_specification.extent_.width = image_wrapper.GetWidth();
  image_specification.extent_.height = image_wrapper.GetHeight();
  image_specification.extent_.depth = 1;
  image_specification.format_ = Format::E_R8G8B8A8_UNORM;
  image_specification.usage_ = ImageUsageMaskBits::E_SAMPLED_BIT | ImageUsageMaskBits::E_TRANSFER_DST_BIT;
  Initialize(ImageType::E_2D, ImageViewType::E_2D, image_specification, sampler_specification);
  SetImageData(image_wrapper.GetData());
}

Image2D::Image2D(uint32_t width, uint32_t height, std::span<const std::byte> data,
                 const std::optional<SamplerSpecification> &sampler_specification) {
  ImageSpecification image_specification;
  image_specification.extent_ = Extent3D(width, height, 1);
  image_specification.format_ = Format::E_R8G8B8A8_UNORM;
  image_specification.usage_ = ImageUsageMaskBits::E_SAMPLED_BIT | ImageUsageMaskBits::E_TRANSFER_DST_BIT;
  Initialize(ImageType::E_2D, ImageViewType::E_2D, image_specification, sampler_specification);
  SetImageData(data);
}

} // namespace Innsmouth