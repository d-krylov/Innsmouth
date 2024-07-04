#ifndef INNSMOUTH_IMAGE_H
#define INNSMOUTH_IMAGE_H

#include "graphics/include/graphics_types.h"

namespace Innsmouth {

class Picture;

class Image {
public:
  Image();

  ~Image();

  operator const VkImage &() const { return image_; }

  [[nodiscard]] const VkExtent3D &GetExtent() const { return extent_; }
  [[nodiscard]] const VkImageView GetImageView() const { return image_view_; }
  [[nodiscard]] const VkSampler GetSampler() const { return sampler_; }
  [[nodiscard]] VkFormat GetFormat() const { return format_; }

  void SetData();

  [[nodiscard]] WriteDescriptorSet GetWriteDescriptorSet(uint32_t binding,
                                                         VkDescriptorType type) const;

  static void CreateImage(VkImage &image, VmaAllocation &allocation, VkImageType image_type,
                          const VkExtent3D &extent, uint32_t levels, uint32_t layers,
                          VkFormat image_format, VkImageTiling tiling, VkImageUsageFlags usage,
                          VkSampleCountFlagBits samples);

  static void
  CreateImageView(const VkImage &image, VkImageView &image_view, VkFormat format,
                  VkImageViewType image_view_type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
                  VkImageAspectFlags aspect = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
                  uint32_t mip_base = 0, uint32_t mip_count = 1, uint32_t base_layer = 0,
                  uint32_t layer_count = 1);

  static void CreateImageSampler(VkSampler &sampler, VkFilter filter,
                                 VkSamplerAddressMode address_mode, uint32_t min_lod,
                                 uint32_t max_lod);
  static void CreateMipmaps();

  static void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout from,
                                    VkImageLayout to, uint32_t levels, uint32_t layers);

protected:
  void CreateImageMemory();

protected:
  VkImage image_{VK_NULL_HANDLE};
  VkImageView image_view_{VK_NULL_HANDLE};
  VkImageLayout image_layout_{VK_IMAGE_LAYOUT_UNDEFINED};
  VkSampler sampler_{VK_NULL_HANDLE};
  VkExtent3D extent_{0, 0, 0};
  VkImageType type_;
  VmaAllocation allocation_{VK_NULL_HANDLE};
  VkFormat format_;
  uint32_t levels_{0};
  uint32_t layers_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_H