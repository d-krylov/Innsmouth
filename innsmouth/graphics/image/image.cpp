#include "image.h"
#include "sampler.h"
#include "innsmouth/graphics/core/structure_tools.h"
#include "innsmouth/graphics/buffer/buffer.h"
#include "innsmouth/graphics/command/command_buffer.h"
#include "innsmouth/graphics/synchronization/fence.h"
#include "innsmouth/core/include/core.h"
#include <print>

namespace Innsmouth {

VkImage Image::CreateImage(ImageType image_type, const ImageSpecification &image_specification, VmaAllocation &out_allocation) {
  ImageCreateInfo image_ci;
  image_ci.imageType = image_type;
  image_ci.extent = image_specification.extent_;
  image_ci.mipLevels = image_specification.levels_;
  image_ci.arrayLayers = image_specification.layers_;
  image_ci.format = image_specification.format_;
  image_ci.tiling = ImageTiling::E_OPTIMAL;
  image_ci.initialLayout = ImageLayout::E_UNDEFINED;
  image_ci.usage = image_specification.usage_;
  image_ci.samples = SampleCountMaskBits::E_1_BIT;
  image_ci.sharingMode = SharingMode::E_EXCLUSIVE;
  VkImage image = VK_NULL_HANDLE;
  auto allocation_information = GraphicsAllocator::Get()->AllocateImage(image_ci, image);
  out_allocation = allocation_information.allocation_;
  return image;
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
  auto aspect_mask = GetAspectMask(GetFormat());
  auto subresource = GetImageSubresourceRange(aspect_mask, 0, GetLevelCoount(), 0, GetLayerCoount());
  image_ = CreateImage(image_type, image_specification_, vma_allocation_);
  image_view_ = CreateImageView(GetImage(), GetFormat(), view_type, subresource);
  image_sampler_ = sampler_specification.has_value() ? Sampler::CreateSampler(sampler_specification.value()) : nullptr;
}

void Image::GenerateMipmaps() {
}

void Image::SetImageLayout(ImageLayout new_layout, CommandBuffer *command_buffer) {
  if (new_layout == current_layout_) return;
  auto access0 = GetAccessMaskFromLayout(current_layout_, false);
  auto access1 = GetAccessMaskFromLayout(new_layout, true);
  auto stage0 = GetPipelineStageMaskFromLayout(current_layout_, false);
  auto stage1 = GetPipelineStageMaskFromLayout(new_layout, true);
  auto subresource = GetImageSubresourceRange(ImageAspectMaskBits::E_COLOR_BIT);
  command_buffer->CommandImageMemoryBarrier(GetImage(), current_layout_, new_layout, stage0, stage1, access0, access1, subresource);
  current_layout_ = new_layout;
}

void Image::SetImageData(std::span<const std::byte> data) {
  Buffer buffer(data.size(), BufferUsageMaskBits::E_TRANSFER_SRC_BIT, Buffer::CPU);
  buffer.SetData<std::byte>(data);
  CommandBuffer command_buffer(GraphicsContext::Get()->GetGraphicsQueueIndex());
  command_buffer.Begin();
  SetImageLayout(ImageLayout::E_TRANSFER_DST_OPTIMAL, &command_buffer);
  command_buffer.CommandCopyBufferToImage(buffer.GetHandle(), GetImage(), GetExtent());
  SetImageLayout(ImageLayout::E_SHADER_READ_ONLY_OPTIMAL, &command_buffer);
  command_buffer.End();
  command_buffer.Submit();
}

Image::Image(Image &&other) noexcept {
  image_ = std::exchange(other.image_, VK_NULL_HANDLE);
  vma_allocation_ = std::exchange(other.vma_allocation_, VK_NULL_HANDLE);
  image_view_ = std::exchange(other.image_view_, VK_NULL_HANDLE);
  image_sampler_ = std::exchange(other.image_sampler_, VK_NULL_HANDLE);
  current_layout_ = std::exchange(other.current_layout_, ImageLayout::E_UNDEFINED);
  image_specification_ = std::exchange(other.image_specification_, ImageSpecification());
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

VkImage Image::GetImage() const {
  return image_;
}

VkImageView Image::GetImageView() const {
  return image_view_;
}

VkSampler Image::GetSampler() const {
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

const Extent3D &Image::GetExtent() const {
  return image_specification_.extent_;
}

ImageLayout Image::GetCurrentLayout() const {
  return current_layout_;
}

DescriptorImageInfo Image::GetDescriptor() const {
  DescriptorImageInfo descriptor_image_info;
  descriptor_image_info.imageLayout = current_layout_;
  descriptor_image_info.imageView = GetImageView();
  descriptor_image_info.sampler = GetSampler();
  return descriptor_image_info;
}

} // namespace Innsmouth