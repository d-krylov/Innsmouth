#ifndef INNSMOUTH_IMAGE_H
#define INNSMOUTH_IMAGE_H

#include "graphics/include/graphics_types.h"

namespace Innsmouth {

struct SamplerAddress {
  SamplerAddress(SamplerAddressMode x) : u_(x), v_(x), w_(x) {}
  SamplerAddress(SamplerAddressMode u, SamplerAddressMode v, SamplerAddressMode w) : u_(u), v_(v), w_(w) {}
  SamplerAddressMode u_;
  SamplerAddressMode v_;
  SamplerAddressMode w_;
};

class Image {
public:
  Image() = default;

  Image(uint32_t width, uint32_t height, uint32_t depth, Format format, uint32_t levels, uint32_t layers,
        SampleCount samples, Filter min, Filter mag, SamplerAddress uvw);

  ~Image();

  NO_COPY_SEMANTIC(Image);

  Image(Image &&other) noexcept;

  operator const VkImage &() const { return image_; }

  [[nodiscard]] const VkExtent3D &GetExtent() const { return extent_; }
  [[nodiscard]] const VkImageView GetImageView() const { return image_view_; }
  [[nodiscard]] const VkSampler GetSampler() const { return sampler_; }
  [[nodiscard]] Format GetFormat() const { return format_; }

  static void CreateImage(VkImage &image, VmaAllocation &allocation, ImageType image_type,
                          const VkExtent3D &extent, uint32_t levels, uint32_t layers, Format format,
                          ImageTiling tiling, ImageUsage usage, SampleCount samples = SampleCount::BIT1);

  // STATIC

  static void CreateImageView(const VkImage &image, VkImageView &image_view, Format format,
                              ImageViewType view_type, const VkImageSubresourceRange &range);

  static void CreateImageSampler(VkSampler &sampler, Filter min, Filter mag, const SamplerAddress &uvw);
  static void CreateMipmaps();

  static void TransitionImageLayout(VkImage image, ImageLayout from, ImageLayout to,
                                    PipelineStage source_stage, PipelineStage destination_stage,
                                    const VkImageSubresourceRange &range);

protected:
  VkImage image_{VK_NULL_HANDLE};
  VkImageView image_view_{VK_NULL_HANDLE};
  VkImageLayout image_layout_{VK_IMAGE_LAYOUT_UNDEFINED};
  VkSampler sampler_{VK_NULL_HANDLE};
  VmaAllocation allocation_{VK_NULL_HANDLE};
  VkExtent3D extent_{0, 0, 0};
  SampleCount samples_{SampleCount::BIT1};
  VkImageType type_;
  Format format_;
  ImageUsage usage_;
  SamplerAddress sampler_address_;
  Filter min_;
  Filter mag_;
  uint32_t levels_{0};
  uint32_t layers_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_H