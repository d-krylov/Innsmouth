#include "image.h"
#include "graphics/include/command_buffer.h"
#include "graphics/include/graphics.h"

namespace Innsmouth {

// clang-format off
Image::Image() {

  
}
// clang-format on

Image::~Image() {}

void Image::CreateImage(VkImage &image, VmaAllocation &allocation, VkImageType image_type,
                        const VkExtent3D &extent, uint32_t levels, uint32_t layers,
                        VkFormat image_format, VkImageTiling tiling, VkImageUsageFlags usage,
                        VkSampleCountFlagBits samples) {
  VkImageCreateInfo image_ci{};
  {
    image_ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_ci.imageType = image_type;
    image_ci.extent = extent;
    image_ci.mipLevels = levels;
    image_ci.arrayLayers = layers;
    image_ci.format = image_format;
    image_ci.tiling = tiling;
    image_ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_ci.usage = usage;
    image_ci.samples = samples;
    image_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  VmaAllocationCreateInfo allocation_ci{};
  {
    allocation_ci.flags = 0;
    allocation_ci.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    allocation_ci.requiredFlags = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    allocation_ci.preferredFlags = 0;
    allocation_ci.memoryTypeBits = 0;
    allocation_ci.pool = nullptr;
    allocation_ci.pUserData = nullptr;
  }

  VmaAllocationInfo allocation_info{};
  VK_CHECK(
    vmaCreateImage(Allocator(), &image_ci, &allocation_ci, &image, &allocation, &allocation_info));
}

void Image::CreateImageView(const VkImage &image, VkImageView &image_view, VkFormat format,
                            VkImageViewType image_view_type, VkImageAspectFlags aspect,
                            uint32_t mip_base, uint32_t mip_count, uint32_t base_layer,
                            uint32_t layer_count) {
  VkImageViewCreateInfo image_view_ci{};
  {
    image_view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_ci.image = image;
    image_view_ci.viewType = image_view_type;
    image_view_ci.format = format;
    image_view_ci.subresourceRange.aspectMask = aspect;
    image_view_ci.subresourceRange.baseMipLevel = mip_base;
    image_view_ci.subresourceRange.levelCount = mip_count;
    image_view_ci.subresourceRange.baseArrayLayer = base_layer;
    image_view_ci.subresourceRange.layerCount = layer_count;
  }
  VK_CHECK(vkCreateImageView(Device(), &image_view_ci, nullptr, &image_view));
}

void Image::CreateImageSampler(VkSampler &sampler, VkFilter filter,
                               VkSamplerAddressMode address_mode, uint32_t min_lod,
                               uint32_t max_lod) {
  VkSamplerCreateInfo sampler_ci{};
  {
    sampler_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_ci.magFilter = filter;
    sampler_ci.minFilter = filter;
    sampler_ci.addressModeU = address_mode;
    sampler_ci.addressModeV = address_mode;
    sampler_ci.addressModeW = address_mode;
    sampler_ci.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_ci.compareEnable = VK_FALSE;
    sampler_ci.unnormalizedCoordinates = VK_FALSE;
    sampler_ci.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_ci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_ci.mipLodBias = 0.0f;
    sampler_ci.minLod = min_lod;
    sampler_ci.maxLod = max_lod;
  }
  VK_CHECK(vkCreateSampler(Device(), &sampler_ci, nullptr, &sampler));
}

void Image::SetData() {}

void Image::CreateMipmaps() {}

WriteDescriptorSet Image::GetWriteDescriptorSet(uint32_t binding, VkDescriptorType type) const {
  VkDescriptorImageInfo descriptor_ii{};
  {
    descriptor_ii.sampler = sampler_;
    descriptor_ii.imageView = image_view_;
    descriptor_ii.imageLayout = image_layout_;
  }
  VkWriteDescriptorSet write_descriptor_set{};
  {
    write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_set.dstSet = VK_NULL_HANDLE;
    write_descriptor_set.dstBinding = binding;
    write_descriptor_set.dstArrayElement = 0;
    write_descriptor_set.descriptorType = type;
    write_descriptor_set.descriptorCount = 1;
  }
  return WriteDescriptorSet(descriptor_ii, write_descriptor_set);
}

void Image::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout from,
                                  VkImageLayout to, uint32_t levels, uint32_t layers) {

  CommandBuffer command_buffer(CommandPool(), true);

  command_buffer.CommandSetImageLayout(image, from, to, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, levels,
                                       layers);

  command_buffer.Flush();
}

} // namespace Innsmouth