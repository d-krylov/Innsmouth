#include "image_2d.h"
#include "graphics/include/buffer.h"
#include "graphics/include/command_buffer.h"
#include "graphics/include/graphics.h"

namespace Innsmouth {

Image2D::Image2D(const CoreImage &core_image) {

  extent_ = VkExtent3D{core_image.GetWidth(), core_image.GetHeight(), 1};

  auto levels = 1; // GetMipLevels(extent);

  Image::CreateImage(image_, allocation_, VkImageType::VK_IMAGE_TYPE_2D, extent_, levels, 1,
                     VkFormat::VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
                     VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT |
                       VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                     samples_);

  Image::CreateImageView(image_, image_view_, VkFormat::VK_FORMAT_R8G8B8A8_UNORM,
                         VkImageViewType::VK_IMAGE_VIEW_TYPE_2D, CreateImageSubresourceRange());

  Image::CreateImageSampler(sampler_, VkFilter::VK_FILTER_LINEAR, VkFilter::VK_FILTER_LINEAR,
                            SamplerAddressMode::CLAMP_TO_EDGE, SamplerAddressMode::CLAMP_TO_EDGE,
                            SamplerAddressMode::CLAMP_TO_EDGE);

  SetData(core_image.GetData());
}

Image2D::Image2D(const VkExtent2D &extent, std::span<std::byte> data) {

  auto levels = 1;

  extent_ = VkExtent3D{extent.width, extent.height, 1};

  Image::CreateImage(image_, allocation_, VkImageType::VK_IMAGE_TYPE_2D, extent_, levels, 1,
                     VkFormat::VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
                     VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT |
                       VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                     samples_);

  Image::CreateImageView(image_, image_view_, VkFormat::VK_FORMAT_R8G8B8A8_UNORM,
                         VkImageViewType::VK_IMAGE_VIEW_TYPE_2D, CreateImageSubresourceRange());

  Image::CreateImageSampler(sampler_, VkFilter::VK_FILTER_LINEAR, VkFilter::VK_FILTER_LINEAR,
                            SamplerAddressMode::REPEAT, SamplerAddressMode::REPEAT,
                            SamplerAddressMode::REPEAT);

  SetData(data);
}

void Image2D::SetData(std::span<std::byte> data) {
  Buffer buffer(BufferUsage::TRANSFER_SRC, data.size());
  buffer.SetData(data);
  CommandBuffer command_buffer(CommandPool(), true);
  auto range = CreateImageSubresourceRange();
  command_buffer.CommandSetImageLayout(image_, ImageLayout::UNDEFINED,
                                       ImageLayout::TRANSFER_DST_OPTIMAL, range);
  command_buffer.CommandCopyBufferToImage(buffer, *this, extent_);
  command_buffer.CommandSetImageLayout(image_, ImageLayout::TRANSFER_DST_OPTIMAL,
                                       ImageLayout::SHADER_READ_ONLY_OPTIMAL, range);
  command_buffer.Flush();
}

} // namespace Innsmouth