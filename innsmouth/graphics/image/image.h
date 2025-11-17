#ifndef INNSMOUTH_IMAGE_H
#define INNSMOUTH_IMAGE_H

#include "sampler.h"
#include "vma/vk_mem_alloc.h"
#include <span>

namespace Innsmouth {

class ImageSpecification {
public:
  ImageViewType view_type_;
  ImageType image_type_;
  ImageUsageMask usage_;
  Extent3D extent_;
  Format format_;
  uint32_t levels_{1};
  uint32_t layers_{1};
  ImageLayout layout_ = ImageLayout::E_UNDEFINED;
};

class Image {
public:
  Image() = default;

  Image(const ImageSpecification &image_specification);

  Image(const ImageSpecification &image_specification, const SamplerSpecification &sampler_specification);

  ~Image();

  Image(const Image &) = delete;
  Image &operator=(const Image &) = delete;

  Image(Image &&other) noexcept;
  Image &operator=(Image &&other) noexcept;

  const VkImage GetImage() const;
  const VkImageView GetImageView() const;
  const VkSampler GetSampler() const;

  void SetImageData(std::span<const std::byte> data);

protected:
  void CreateImage();
  void CreateImageView();
  void SetLayout(ImageLayout destination_layout);

private:
  VkImage image_{VK_NULL_HANDLE};
  VmaAllocation vma_allocation_{VK_NULL_HANDLE};
  VkImageView image_view_{VK_NULL_HANDLE};
  VkSampler image_sampler_{VK_NULL_HANDLE};
  ImageSpecification image_specification_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_H