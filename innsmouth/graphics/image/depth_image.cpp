#include "depth_image.h"
#include "innsmouth/graphics/include/graphics.h"

namespace Innsmouth {

[[nodiscard]] VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates,
                                           VkImageTiling tiling, VkFormatFeatureFlags features) {
  VkFormatProperties fp;
  for (auto format : candidates) {
    vkGetPhysicalDeviceFormatProperties(PhysicalDevice(), format, &fp);
    if (tiling == VK_IMAGE_TILING_LINEAR && (fp.linearTilingFeatures & features) == features) {
      return format;
    }
    if (tiling == VK_IMAGE_TILING_OPTIMAL && (fp.optimalTilingFeatures & features) == features) {
      return format;
    }
  }
  return VK_FORMAT_UNDEFINED;
}

[[nodiscard]] std::vector<VkFormat> GetDepthFormats() {
  return std::vector<VkFormat>{VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT,
                               VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT,
                               VK_FORMAT_D16_UNORM};
}

DepthImage::DepthImage(const VkExtent2D &extent, VkSampleCountFlagBits samples)
  : Image({extent.width, extent.height, 1},
          FindSupportedFormat(GetDepthFormats(), VK_IMAGE_TILING_OPTIMAL,
                              VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT),
          1, 1, samples, Filter::LINEAR, Filter::LINEAR, SamplerAddressMode::CLAMP_TO_EDGE,
          SamplerAddressMode::CLAMP_TO_EDGE, SamplerAddressMode::CLAMP_TO_EDGE) {

  CORE_VERIFY(format_ != VK_FORMAT_UNDEFINED);

  VkImageAspectFlags aspect = VK_IMAGE_ASPECT_DEPTH_BIT;

  aspect |= FormatHasStencil(format_) ? VK_IMAGE_ASPECT_STENCIL_BIT : 0;

  CreateImage(image_, allocation_, VK_IMAGE_TYPE_2D, extent_, levels_, layers_, format_,
              VK_IMAGE_TILING_OPTIMAL, ImageUsage::SAMPLED | ImageUsage::DEPTH_STENCIL_ATTACHMENT);

  CreateImageView(image_, image_view_, format_, VK_IMAGE_VIEW_TYPE_2D,
                  CreateImageSubresourceRange(ImageAspect::DEPTH));

  CreateImageSampler(sampler_, min_, mag_, address_mode_[0], address_mode_[1], address_mode_[2]);
}

} // namespace Innsmouth