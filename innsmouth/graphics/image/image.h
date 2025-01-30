#ifndef INNSMOUTH_IMAGE_H
#define INNSMOUTH_IMAGE_H

#include "graphics/include/graphics_types.h"

namespace Innsmouth {

class Image {
public:
  static void CreateImageView(const VkImage &image, VkImageView &image_view, Format format, ImageViewType image_view_type,
                              const VkImageSubresourceRange &range);

private:
  VkImage image_{VK_NULL_HANDLE};
  VkImageView image_view_{VK_NULL_HANDLE};
  VkSampler image_sampler_{VK_NULL_HANDLE};
  VmaAllocation vma_allocation_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_H