#include "image.h"
#include "graphics/include/command_buffer.h"
#include "graphics/include/graphics.h"
#include <iostream>

namespace Innsmouth {

Image::Image(uint32_t width, uint32_t height, uint32_t depth, Format format, uint32_t levels, uint32_t layers,
             SampleCount samples, Filter min, Filter mag, SamplerAddress uvw)
  : extent_{width, height, depth}, format_(format), levels_(levels), layers_(layers), sampler_address_(uvw),
    min_(min), mag_(mag) {}

Image::~Image() {
  if (sampler_) {
    vkDestroySampler(Device(), sampler_, nullptr);
  }
  if (image_view_) {
    vkDestroyImageView(Device(), image_view_, nullptr);
  }
  if (image_) {
    vmaDestroyImage(Allocator(), image_, allocation_);
  }
}

Image::Image(Image &&other) noexcept
  : image_(std::exchange(other.image_, VK_NULL_HANDLE)),
    image_view_(std::exchange(other.image_view_, VK_NULL_HANDLE)), image_layout_(other.image_layout_),
    sampler_(std::exchange(other.sampler_, VK_NULL_HANDLE)), allocation_(other.allocation_),
    extent_(other.extent_), samples_(other.samples_), type_(other.type_), format_(other.format_),
    usage_(other.usage_), sampler_address_(other.sampler_address_), min_(other.min_), mag_(other.mag_),
    levels_(other.levels_), layers_(other.layers_) {

  std::cout << image_view_ << std::endl;
}

void Image::CreateImage(VkImage &image, VmaAllocation &allocation, ImageType image_type,
                        const VkExtent3D &extent, uint32_t levels, uint32_t layers, Format format,
                        ImageTiling tiling, ImageUsage usage, SampleCount samples) {
  VkImageCreateInfo image_ci{};
  {
    image_ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_ci.imageType = VkImageType(image_type);
    image_ci.extent = extent;
    image_ci.mipLevels = levels;
    image_ci.arrayLayers = layers;
    image_ci.format = VkFormat(format);
    image_ci.tiling = VkImageTiling(tiling);
    image_ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_ci.usage = VkImageUsageFlags(usage);
    image_ci.samples = VkSampleCountFlagBits(samples);
    image_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

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

void Image::CreateImageView(const VkImage &image, VkImageView &image_view, Format format,
                            ImageViewType image_view_type, const VkImageSubresourceRange &range) {
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

void Image::CreateImageSampler(VkSampler &sampler, Filter min, Filter mag, const SamplerAddress &uvw) {
  VkSamplerCreateInfo sampler_ci{};
  {
    sampler_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_ci.magFilter = VkFilter(mag);
    sampler_ci.minFilter = VkFilter(min);
    sampler_ci.addressModeU = VkSamplerAddressMode(uvw.u_);
    sampler_ci.addressModeV = VkSamplerAddressMode(uvw.v_);
    sampler_ci.addressModeW = VkSamplerAddressMode(uvw.w_);
    sampler_ci.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_ci.compareEnable = VK_FALSE;
    sampler_ci.unnormalizedCoordinates = VK_FALSE;
    sampler_ci.compareOp = VK_COMPARE_OP_NEVER;
    sampler_ci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_ci.mipLodBias = 0.0f;
    sampler_ci.minLod = -1000.0f;
    sampler_ci.maxLod = 1000.0f;
    sampler_ci.maxAnisotropy = 1.0f;
  }
  VK_CHECK(vkCreateSampler(Device(), &sampler_ci, nullptr, &sampler));
}

void Image::CreateMipmaps() {}

void Image::TransitionImageLayout(VkImage image, ImageLayout from, ImageLayout to, PipelineStage source_stage,
                                  PipelineStage destination_stage, const VkImageSubresourceRange &range) {
  CommandBuffer command_buffer(CommandPool(), true);
  command_buffer.CommandSetImageLayout(image, from, to, range, source_stage, destination_stage);
  command_buffer.Flush();
}

} // namespace Innsmouth