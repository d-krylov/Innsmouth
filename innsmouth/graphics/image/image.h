#ifndef INNSMOUTH_IMAGE_H
#define INNSMOUTH_IMAGE_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"
#include "vma/vk_mem_alloc.h"
#include <span>

namespace Innsmouth {

class ImageSpecification {
public:
  ImageSpecification(uint32_t width, uint32_t height);

  VkExtent3D GetExtent3D() const;
  uint32_t GetLevels() const;
  uint32_t GetLayers() const;

public:
  uint32_t levels_{1};
  uint32_t width_{0};
  uint32_t height_{0};
  uint32_t depth_{1};
};

class Image {
public:
  Image(VkImageUsageFlags image_usage, VkImageType image_type, VkFormat image_format, const ImageSpecification &image_information);

  ~Image();

  template <typename T> void SetData(std::span<const T> data);

  const VkImage GetImage() const;
  const VkImageView GetImageView() const;
  const VkSampler GetSampler() const;

protected:
  void CreateImage();
  void CreateImageView();
  void SetImageData(std::span<const std::byte> data);

private:
  VkImage image_{VK_NULL_HANDLE};
  VmaAllocation vma_allocation_{VK_NULL_HANDLE};
  VkImageView image_view_{VK_NULL_HANDLE};
  VkSampler image_sampler_{VK_NULL_HANDLE};
  VkImageUsageFlags image_usage_;
  VkImageType image_type_;
  VkFormat image_format_;
  ImageSpecification image_information_;
};

} // namespace Innsmouth

#include "image.ipp"

#endif // INNSMOUTH_IMAGE_H