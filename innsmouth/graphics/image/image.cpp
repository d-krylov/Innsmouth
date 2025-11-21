#include "image.h"
#include "sampler.h"
#include "innsmouth/graphics/core/structure_tools.h"
#include "innsmouth/graphics/buffer/buffer.h"
#include "innsmouth/graphics/command/command_buffer.h"
#include "innsmouth/graphics/synchronization/fence.h"

namespace Innsmouth {

Image::Image(const ImageSpecification &image_specification) : image_specification_(image_specification) {
  CreateImage();
  CreateImageView();
}

Image::Image(const ImageSpecification &image_specification, const SamplerSpecification &sampler_specification) : Image(image_specification) {
  image_sampler_ = Sampler::CreateVulkanSampler(sampler_specification);
}

Image::~Image() {
  vkDestroyImageView(GraphicsContext::Get()->GetDevice(), GetImageView(), nullptr);
  GraphicsAllocator::Get()->DestroyImage(image_, vma_allocation_);
}

void Image::CreateImage() {
  ImageCreateInfo image_ci;
  {
    image_ci.imageType = image_specification_.image_type_;
    image_ci.extent = image_specification_.extent_;
    image_ci.mipLevels = image_specification_.levels_;
    image_ci.arrayLayers = image_specification_.layers_;
    image_ci.format = image_specification_.format_;
    image_ci.tiling = ImageTiling::E_OPTIMAL;
    image_ci.initialLayout = image_specification_.layout_;
    image_ci.usage = image_specification_.usage_;
    image_ci.samples = SampleCountMaskBits::E_1_BIT;
    image_ci.sharingMode = SharingMode::E_EXCLUSIVE;
  }

  vma_allocation_ = GraphicsAllocator::Get()->AllocateImage(image_ci, &image_);
}

void Image::CreateImageView() {

  ImageAspectMask aspect =
    (image_specification_.format_ == Format::E_D32_SFLOAT) ? ImageAspectMaskBits::E_DEPTH_BIT : ImageAspectMaskBits::E_COLOR_BIT;

  ImageViewCreateInfo image_view_ci;
  {
    image_view_ci.image = image_;
    image_view_ci.viewType = image_specification_.view_type_;
    image_view_ci.format = image_specification_.format_;
    image_view_ci.subresourceRange = GetImageSubresourceRange(aspect);
  }

  VK_CHECK(vkCreateImageView(GraphicsContext::Get()->GetDevice(), image_view_ci, nullptr, &image_view_));
}

void Image::SetLayout(ImageLayout destination_layout) {
}

void Image::SetImageData(std::span<const std::byte> data) {
  Buffer buffer(data.size(), BufferUsageMaskBits::E_TRANSFER_SRC_BIT, AllocationCreateMaskBits::E_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
  buffer.SetData<std::byte>(data);

  CommandBuffer command_buffer(GraphicsContext::Get()->GetGraphicsQueueIndex());
  command_buffer.Begin();

  ImageAspectMask aspect =
    (image_specification_.format_ == Format::E_D32_SFLOAT) ? ImageAspectMaskBits::E_DEPTH_BIT : ImageAspectMaskBits::E_COLOR_BIT;

  auto image_subresource_range = GetImageSubresourceRange(aspect);

  command_buffer.CommandImageMemoryBarrier(image_, ImageLayout::E_UNDEFINED, ImageLayout::E_TRANSFER_DST_OPTIMAL,
                                           PipelineStageMaskBits2::E_TOP_OF_PIPE_BIT, PipelineStageMaskBits2::E_ALL_TRANSFER_BIT,
                                           image_subresource_range);

  command_buffer.CommandCopyBufferToImage(buffer.GetHandle(), image_, image_specification_.extent_);

  command_buffer.CommandImageMemoryBarrier(image_, ImageLayout::E_TRANSFER_DST_OPTIMAL, ImageLayout::E_SHADER_READ_ONLY_OPTIMAL,
                                           PipelineStageMaskBits2::E_ALL_TRANSFER_BIT, PipelineStageMaskBits2::E_ALL_COMMANDS_BIT,
                                           image_subresource_range);

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

Image::Image(Image &&other) noexcept {
  image_ = std::exchange(other.image_, VK_NULL_HANDLE);
  vma_allocation_ = std::exchange(other.vma_allocation_, VK_NULL_HANDLE);
  image_view_ = std::exchange(other.image_view_, VK_NULL_HANDLE);
  image_sampler_ = std::exchange(other.image_sampler_, VK_NULL_HANDLE);
}

Image &Image::operator=(Image &&other) noexcept {
  std::swap(image_, other.image_);
  std::swap(vma_allocation_, other.vma_allocation_);
  std::swap(image_view_, other.image_view_);
  std::swap(image_sampler_, other.image_sampler_);
  std::swap(image_specification_, other.image_specification_);
  return *this;
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

} // namespace Innsmouth