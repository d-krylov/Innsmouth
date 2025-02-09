#ifndef INNSMOUTH_IMAGE_H
#define INNSMOUTH_IMAGE_H

#include "graphics/include/graphics_types.h"
#include <span>

namespace Innsmouth {

class Image {
public:
  Image(uint32_t width, uint32_t height, uint32_t depth);

  operator const VkImage &() const { return image_; }

  const VkExtent3D &GetExtent() const { return extent_; }
  const VkImageView GetImageView() const { return image_view_; }
  const VkSampler GetSampler() const { return image_sampler_; }

  void SetData(std::span<const std::byte> data);

  static VkImage CreateImage(VmaAllocation &allocation, const VkImageCreateInfo &image_ci);
  static VkImageView CreateImageView(const VkImage image, Format format, ImageViewType type, const VkImageSubresourceRange &range);

protected:
  VkImage image_{VK_NULL_HANDLE};
  VkImageView image_view_{VK_NULL_HANDLE};
  VkSampler image_sampler_{VK_NULL_HANDLE};
  VmaAllocation vma_allocation_{VK_NULL_HANDLE};
  VkExtent3D extent_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_H