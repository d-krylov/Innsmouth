#include "image.h"
#include "sampler.h"
#include "innsmouth/graphics/core/structure_tools.h"
#include "innsmouth/graphics/buffer/buffer.h"
#include "innsmouth/graphics/command/command_buffer.h"
#include "innsmouth/graphics/synchronization/fence.h"

namespace Innsmouth {

Image::Image(ImageType type, ImageViewType view_type, const ImageSpecification &specification,
             const std::optional<SamplerSpecification> &sampler_specification) {
  Initialize(type, view_type, specification, sampler_specification);
}

Image::~Image() {
  vkDestroyImageView(GraphicsContext::Get()->GetDevice(), GetImageView(), nullptr);
  GraphicsAllocator::Get()->DestroyImage(image_, vma_allocation_);
}

void Image::Initialize(ImageType image_type, ImageViewType view_type, const ImageSpecification &image_specification,
                       const std::optional<SamplerSpecification> &sampler_specification) {
  image_specification_ = image_specification;
  CreateImage(image_type);
  auto aspect_mask = GetAspectMask(GetFormat());
  auto subresource = GetImageSubresourceRange(aspect_mask, 0, GetLevelCoount(), 0, GetLayerCoount());
  image_view_ = CreateImageView(GetImage(), GetFormat(), view_type, subresource);
  image_sampler_ = sampler_specification.has_value() ? Sampler::CreateSampler(sampler_specification.value()) : nullptr;
}

void Image::CreateImage(ImageType image_type) {
  ImageCreateInfo image_ci;
  image_ci.imageType = image_type;
  image_ci.extent = image_specification_.extent_;
  image_ci.mipLevels = image_specification_.levels_;
  image_ci.arrayLayers = image_specification_.layers_;
  image_ci.format = image_specification_.format_;
  image_ci.tiling = ImageTiling::E_OPTIMAL;
  image_ci.initialLayout = ImageLayout::E_UNDEFINED;
  image_ci.usage = image_specification_.usage_;
  image_ci.samples = SampleCountMaskBits::E_1_BIT;
  image_ci.sharingMode = SharingMode::E_EXCLUSIVE;
  vma_allocation_ = GraphicsAllocator::Get()->AllocateImage(image_ci, &image_);
}

VkImageView Image::CreateImageView(VkImage image, Format format, ImageViewType image_view_type, const ImageSubresourceRange &subresource_range) {
  ImageViewCreateInfo image_view_ci;
  image_view_ci.image = image;
  image_view_ci.viewType = image_view_type;
  image_view_ci.format = format;
  image_view_ci.subresourceRange = subresource_range;
  VkImageView image_view = VK_NULL_HANDLE;
  VK_CHECK(vkCreateImageView(GraphicsContext::Get()->GetDevice(), image_view_ci, nullptr, &image_view));
  return image_view;
}

void Image::SetLayout(ImageLayout destination_layout) {
}

void Image::GenerateMipmaps() {
}

void Image::SetImageData(std::span<const std::byte> data) {
  Buffer buffer(data.size(), BufferUsageMaskBits::E_TRANSFER_SRC_BIT, AllocationCreateMaskBits::E_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
  buffer.SetData<std::byte>(data);

  CommandBuffer command_buffer(GraphicsContext::Get()->GetGraphicsQueueIndex());
  command_buffer.Begin();

  ImageAspectMask aspect = GetAspectMask(GetFormat());

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
  std::swap(current_layout_, other.current_layout_);
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

Format Image::GetFormat() const {
  return image_specification_.format_;
}

uint32_t Image::GetLevelCoount() const {
  return image_specification_.levels_;
}

uint32_t Image::GetLayerCoount() const {
  return image_specification_.layers_;
}

ImageUsageMask Image::GetUsage() const {
  return image_specification_.usage_;
}

DescriptorImageInfo Image::GetDescriptor() const {
  DescriptorImageInfo descriptor_image_info;
  descriptor_image_info.imageLayout = ImageLayout::E_GENERAL;
  descriptor_image_info.imageView = GetImageView();
  descriptor_image_info.sampler = GetSampler();
  return descriptor_image_info;
}

} // namespace Innsmouth