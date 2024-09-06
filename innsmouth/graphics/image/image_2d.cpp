#include "image_2d.h"
#include "innsmouth/graphics/buffer/buffer.h"
#include "innsmouth/graphics/include/command_buffer.h"
#include "innsmouth/graphics/include/graphics.h"

namespace Innsmouth {

Image2D::Image2D(const CoreImage &core_image)
  : Image2D({core_image.GetWidth(), core_image.GetHeight()}, core_image.GetData()) {}

Image2D::Image2D(const VkExtent2D &extent, std::span<const std::byte> data)
  : Image({extent.width, extent.height, 1}, VkFormat::VK_FORMAT_R8G8B8A8_UNORM, 1, 1,
          VK_SAMPLE_COUNT_1_BIT, Filter::LINEAR, Filter::LINEAR, SamplerAddressMode::REPEAT,
          SamplerAddressMode::REPEAT, SamplerAddressMode::CLAMP_TO_EDGE) {

  Image::CreateImage(image_, allocation_, VkImageType::VK_IMAGE_TYPE_2D, extent_, levels_, layers_,
                     format_, VK_IMAGE_TILING_OPTIMAL,
                     ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST, samples_);

  Image::CreateImageView(image_, image_view_, format_, VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
                         CreateImageSubresourceRange());

  Image::CreateImageSampler(sampler_, min_, mag_, address_mode_[0], address_mode_[1],
                            address_mode_[2]);

  SetData(data, extent.width * extent.height * 4);
}

void Image2D::SetData(std::span<const std::byte> data, std::size_t buffer_size) {
  Buffer buffer(BufferUsage::TRANSFER_SRC, (buffer_size == 0) ? data.size() : buffer_size);
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