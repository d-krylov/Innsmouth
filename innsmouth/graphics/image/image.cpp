#include "image.h"

namespace Innsmouth {

ImageInformation::ImageInformation(uint32_t width, uint32_t height) : width_(width), height_(height) {
}

VkExtent3D ImageInformation::GetExtent3D() const {
  return VkExtent3D(width_, height_, depth_);
}

uint32_t ImageInformation::GetLevels() const {
  return levels_;
}

uint32_t ImageInformation::GetLayers() const {
  return layers_;
}

Image::Image(const ImageInformation &image_information) : image_information_(image_information) {
}

Image::~Image() {
}

void Image::CreateImage() {
  VkImageCreateInfo image_ci{};
  {
    image_ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_ci.imageType = image_type_;
    image_ci.extent = image_information_.GetExtent3D();
    image_ci.mipLevels = image_information_.GetLevels();
    image_ci.arrayLayers = image_information_.GetLayers();
    image_ci.format = image_format_;
    image_ci.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //image_ci.usage = VkImageUsageFlags(usage);
    //image_ci.samples = VkSampleCountFlagBits(samples);
    image_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }
}

void Image::CreateImageView() {
  VkImageSubresourceRange image_subresource_range{};
  {
    image_subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_subresource_range.baseArrayLayer = 0;
    image_subresource_range.layerCount = 1;
    image_subresource_range.baseMipLevel = 0;
    image_subresource_range.levelCount = 1;
  }

  VkImageViewCreateInfo image_view_ci{};
  {
    image_view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_ci.image = image_;
    image_view_ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_ci.format = VK_FORMAT_UNDEFINED;
    image_view_ci.subresourceRange = image_subresource_range;
  }

  VK_CHECK(vkCreateImageView(GraphicsContext::Get().GetDevice(), &image_view_ci, nullptr, &image_view_));
}

void Image::SetData() {

}

} // namespace Innsmouth