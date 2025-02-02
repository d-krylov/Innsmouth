#include "image.h"
#include "graphics/include/graphics.h"

namespace Innsmouth {

void Image::CreateImage(VkImage &image, VmaAllocation &allocation, const VkImageCreateInfo &image_ci) {
  VmaAllocationCreateInfo allocation_ci{};
  {
    allocation_ci.flags = 0;
    allocation_ci.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    allocation_ci.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    allocation_ci.preferredFlags = 0;
    allocation_ci.memoryTypeBits = 0;
    allocation_ci.pool = nullptr;
    allocation_ci.pUserData = nullptr;
  }

  VmaAllocationInfo allocation_info{};

  VK_CHECK(vmaCreateImage(Allocator(), &image_ci, &allocation_ci, &image, &allocation, &allocation_info));
}

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

void Image::SetData(std::span<const std::byte> data) {}

} // namespace Innsmouth