#include "image.h"
#include "graphics/include/graphics.h"

namespace Innsmouth {

void Image::CreateImageView(const VkImage &image, VkImageView &image_view, Format format, ImageViewType image_view_type,
                            const VkImageSubresourceRange &range) {
  VkImageViewCreateInfo image_view_ci{};
  {
    image_view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_ci.image = image;
    image_view_ci.viewType = VkImageViewType(image_view_type);
    image_view_ci.format = VkFormat(format);
    image_view_ci.subresourceRange = range;
  }
  VK_CHECK(vkCreateImageView(Device(), &image_view_ci, nullptr, &image_view));
}

} // namespace Innsmouth