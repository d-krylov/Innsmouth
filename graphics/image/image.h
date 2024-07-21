#ifndef INNSMOUTH_IMAGE_H
#define INNSMOUTH_IMAGE_H

#include "graphics/descriptors/write_descriptor_set.h"
#include "graphics/include/graphics_types.h"

namespace Innsmouth {

class Picture;

class Image {
public:
  Image(const VkExtent3D &extent, VkFormat format, uint32_t levels, uint32_t layers,
        VkSampleCountFlagBits samples, Filter min, Filter mag, SamplerAddressMode u,
        SamplerAddressMode v, SamplerAddressMode w);

  ~Image();

  NO_COPY_SEMANTIC(Image);

  operator const VkImage &() const { return image_; }

  [[nodiscard]] const VkExtent3D &GetExtent() const { return extent_; }
  [[nodiscard]] const VkImageView GetImageView() const { return image_view_; }
  [[nodiscard]] const VkSampler GetSampler() const { return sampler_; }
  [[nodiscard]] VkFormat GetFormat() const { return format_; }

  [[nodiscard]] WriteDescriptorSet
  GetWriteDescriptorSet(uint32_t binding,
                        DescriptorType type = DescriptorType::COMBINED_IMAGE_SAMPLER) const;

  static void
  CreateImage(VkImage &image, VmaAllocation &allocation, VkImageType image_type,
              const VkExtent3D &extent, uint32_t levels, uint32_t layers, VkFormat image_format,
              VkImageTiling tiling, ImageUsage usage,
              VkSampleCountFlagBits samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT);

  // STATIC

  static WriteDescriptorSet
  GetWriteDescriptorSet(const std::vector<VkDescriptorImageInfo> &descriptor_ii, uint32_t binding,
                        DescriptorType type = DescriptorType::COMBINED_IMAGE_SAMPLER);

  static void CreateImageView(const VkImage &image, VkImageView &image_view, VkFormat format,
                              VkImageViewType view_type, const VkImageSubresourceRange &range);

  static void CreateImageSampler(VkSampler &sampler, Filter min, Filter mag, SamplerAddressMode u,
                                 SamplerAddressMode v, SamplerAddressMode w);
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
  VkSampleCountFlagBits samples_{VK_SAMPLE_COUNT_1_BIT};
  VkImageType type_;
  VkFormat format_;
  VkImageUsageFlags usage_;
  std::array<SamplerAddressMode, 3> address_mode_;
  Filter min_;
  Filter mag_;
  uint32_t levels_{0};
  uint32_t layers_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_H