#include "image.h"
#include "innsmouth/graphics/buffer/buffer.h"
#include "innsmouth/graphics/command/command_buffer.h"
#include "innsmouth/graphics/synchronization/fence.h"

namespace Innsmouth {

ImageSpecification::ImageSpecification(uint32_t width, uint32_t height) : width_(width), height_(height) {
}

VkExtent3D ImageSpecification::GetExtent3D() const {
  return VkExtent3D(width_, height_, depth_);
}

uint32_t ImageSpecification::GetLevels() const {
  return levels_;
}

uint32_t ImageSpecification::GetLayers() const {
  return depth_;
}

Image::Image(VkImageUsageFlags image_usage, VkImageType image_type, VkFormat image_format, const ImageSpecification &image_information)
  : image_usage_(image_usage), image_type_(image_type), image_format_(image_format), image_information_(image_information) {
  CreateImage();
  CreateImageView();
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
    image_ci.usage = image_usage_;
    image_ci.samples = VK_SAMPLE_COUNT_1_BIT;
    image_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  VkSamplerCreateInfo sampler_ci{};
  {
    sampler_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_ci.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_ci.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_ci.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_ci.minFilter = VK_FILTER_LINEAR;
    sampler_ci.magFilter = VK_FILTER_LINEAR;
  }

  vma_allocation_ = GraphicsAllocator::Get()->AllocateImage(image_ci, &image_);

  VK_CHECK(vkCreateSampler(GraphicsContext::Get()->GetDevice(), &sampler_ci, nullptr, &image_sampler_));
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
    image_view_ci.format = image_format_;
    image_view_ci.subresourceRange = image_subresource_range;
  }

  VK_CHECK(vkCreateImageView(GraphicsContext::Get()->GetDevice(), &image_view_ci, nullptr, &image_view_));
}

const VkImage Image::GetImage() const {
  return image_;
}

const VkImageView Image::GetImageView() const {
  return image_view_;
}

const VkSampler Image::GetSampler() const {
  return image_sampler_;
}

void Image::SetImageData(std::span<const std::byte> data) {
  Buffer buffer(data.size(), BufferUsageMaskBits::E_TRANSFER_SRC_BIT);
  buffer.Map();
  std::copy(data.begin(), data.end(), buffer.GetMappedData<std::byte>().begin());
  buffer.Unmap();

  CommandBuffer command_buffer(GraphicsContext::Get()->GetGeneralCommandPool());
  command_buffer.Begin();

  VkImageSubresourceRange image_subresource_range{};
  {
    image_subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_subresource_range.baseArrayLayer = 0;
    image_subresource_range.layerCount = 1;
    image_subresource_range.baseMipLevel = 0;
    image_subresource_range.levelCount = 1;
  }

  command_buffer.CommandImageMemoryBarrier(image_, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                           VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, image_subresource_range);

  command_buffer.CommandCopyBufferToImage(buffer.GetHandle(), image_, image_information_.GetExtent3D());

  command_buffer.CommandImageMemoryBarrier(image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                           VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, image_subresource_range);

  command_buffer.End();

  VkSubmitInfo submit_info{};
  {
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = command_buffer.get();
  }

  Fence fence({});

  VK_CHECK(vkQueueSubmit(GraphicsContext::Get()->GetGeneralQueue(), 1, &submit_info, fence.GetHandle()));

  fence.Wait();
}

} // namespace Innsmouth