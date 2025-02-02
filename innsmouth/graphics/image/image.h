#ifndef INNSMOUTH_IMAGE_H
#define INNSMOUTH_IMAGE_H

#include "graphics/include/graphics_types.h"
#include <span>

namespace Innsmouth {

class Image {
public:
  const VkExtent3D &GetExtent() const { return extent_; }

  void SetData(std::span<const std::byte> data);

  static void CreateImage(VkImage &image, VmaAllocation &allocation, const VkImageCreateInfo &image_ci);
  static void CreateImageView(const VkImage &image, VkImageView &image_view, Format format, ImageViewType image_view_type,
                              const VkImageSubresourceRange &range);

private:
  VkImage image_{VK_NULL_HANDLE};
  VkImageView image_view_{VK_NULL_HANDLE};
  VkSampler image_sampler_{VK_NULL_HANDLE};
  VmaAllocation vma_allocation_{VK_NULL_HANDLE};
  VkExtent3D extent_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_H