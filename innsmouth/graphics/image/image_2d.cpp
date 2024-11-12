#include "image_2d.h"
#include "graphics/buffer/buffer.h"
#include "graphics/include/command_buffer.h"
#include "graphics/include/graphics.h"

namespace Innsmouth {

Image2D::Image2D(const CoreImage &core_image)
  : Image2D(core_image.GetWidth(), core_image.GetHeight(), core_image.GetData()) {}

Image2D::Image2D(uint32_t width, uint32_t height, std::span<const std::byte> data)
  : Image(width, height, 1, Format::R8G8B8A8_UNORM, 1, 1, SampleCount::BIT1, Filter::LINEAR, Filter::LINEAR,
          SamplerAddressMode::REPEAT) {

  Image::CreateImage(image_, allocation_, ImageType::_2D, extent_, levels_, layers_, format_,
                     ImageTiling::OPTIMAL, ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST, samples_);

  Image::CreateImageView(image_, image_view_, format_, ImageViewType::_2D, CreateImageSubresourceRange());

  Image::CreateImageSampler(sampler_, min_, mag_, sampler_address_);

  SetData(data, width * height * 4);
}

void Image2D::SetData(std::span<const std::byte> data, std::size_t buffer_size) {
  Buffer buffer(BufferUsage::TRANSFER_SRC, (buffer_size == 0) ? data.size() : buffer_size);
  buffer.SetData(data);
  CommandBuffer command_buffer(CommandPool(), true);
  auto range = CreateImageSubresourceRange();
  command_buffer.CommandSetImageLayout(image_, ImageLayout::UNDEFINED, ImageLayout::TRANSFER_DST_OPTIMAL,
                                       range);
  command_buffer.CommandCopyBufferToImage(buffer, *this, extent_);
  command_buffer.CommandSetImageLayout(image_, ImageLayout::TRANSFER_DST_OPTIMAL,
                                       ImageLayout::SHADER_READ_ONLY_OPTIMAL, range);
  command_buffer.Flush();
}

} // namespace Innsmouth