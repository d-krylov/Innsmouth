#ifndef INNSMOUTH_IMAGE_H
#define INNSMOUTH_IMAGE_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"
#include "vma/vk_mem_alloc.h"

namespace Innsmouth {

class ImageInformation {
public:
  ImageInformation(uint32_t width, uint32_t height);

  VkExtent3D GetExtent3D() const;
  uint32_t GetLevels() const;
  uint32_t GetLayers() const;

public:
  uint32_t levels_{1};
  uint32_t layers_{1};
  uint32_t width_;
  uint32_t height_;
  uint32_t depth_;
};

class Image {
public:
  Image(const ImageInformation &image_information);

  ~Image();

  void SetData();

protected:
  void CreateImage();
  void CreateImageView();

private:
  VkImage image_{VK_NULL_HANDLE};
  VmaAllocation vma_allocation_{VK_NULL_HANDLE};
  VkImageView image_view_{VK_NULL_HANDLE};
  VkImageType image_type_;
  VkFormat image_format_;
  ImageInformation image_information_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_H