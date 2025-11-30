#ifndef INNSMOUTH_IMAGE_H
#define INNSMOUTH_IMAGE_H

#include "sampler.h"
#include "vma/vk_mem_alloc.h"
#include <optional>
#include <span>

namespace Innsmouth {

class CommandBuffer;

struct ImageSpecification {
  Format format_;
  Extent3D extent_;
  uint32_t levels_{1};
  uint32_t layers_{1};
  ImageUsageMask usage_;
};

class Image {
public:
  Image() = default;

  Image(ImageType type, ImageViewType view_type, const ImageSpecification &image_specification,
        const std::optional<SamplerSpecification> &sampler_specification);

  virtual ~Image();

  Image(const Image &) = delete;
  Image &operator=(const Image &) = delete;

  Image(Image &&other) noexcept;
  Image &operator=(Image &&other) noexcept;

  VkImage GetImage() const;
  VkImageView GetImageView() const;
  VkSampler GetSampler() const;

  DescriptorImageInfo GetDescriptor() const;

  Format GetFormat() const;
  uint32_t GetLevelCoount() const;
  uint32_t GetLayerCoount() const;
  ImageUsageMask GetUsage() const;
  const Extent3D &GetExtent() const;
  ImageLayout GetCurrentLayout() const;

  static VkImageView CreateImageView(VkImage image, Format format, ImageViewType image_view_type, const ImageSubresourceRange &subresource);
  static VkImage CreateImage(ImageType image_type, const ImageSpecification &image_specification, VmaAllocation &out_allocation);

  void SetImageData(std::span<const std::byte> data);
  void SetImageLayout(ImageLayout new_layout, CommandBuffer *command_buffer);

protected:
  void Initialize(ImageType type, ImageViewType view_type, const ImageSpecification &image_specification,
                  const std::optional<SamplerSpecification> &sampler_specification = std::nullopt);

  void CreateImage(ImageType image_type);

  void SetLayout(ImageLayout destination_layout);

  void GenerateMipmaps();

private:
  VkImage image_{VK_NULL_HANDLE};
  VmaAllocation vma_allocation_{VK_NULL_HANDLE};
  VkImageView image_view_{VK_NULL_HANDLE};
  VkSampler image_sampler_{VK_NULL_HANDLE};
  ImageLayout current_layout_ = ImageLayout::E_UNDEFINED;
  ImageSpecification image_specification_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_H