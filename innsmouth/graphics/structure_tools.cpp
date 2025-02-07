#include "graphics/include/structure_tools.h"

namespace Innsmouth {

VkImageSubresourceRange CreateImageSubresourceRange(ImageAspect aspect, uint32_t base_level, uint32_t levels, uint32_t base_layer,
                                                    uint32_t layers) {
  VkImageSubresourceRange image_subresource_range{};
  {
    image_subresource_range.aspectMask = std::to_underlying(aspect);
    image_subresource_range.baseMipLevel = base_level;
    image_subresource_range.levelCount = levels;
    image_subresource_range.baseArrayLayer = base_layer;
    image_subresource_range.layerCount = layers;
  }
  return image_subresource_range;
}

VkImageCreateInfo CreateImageCreateInfo(uint32_t width, uint32_t height, uint32_t depth, uint32_t levels, uint32_t layers, Format format,
                                        ImageType image_type, ImageUsage usage, ImageTiling tiling, SampleCount samples) {
  VkImageCreateInfo image_ci{};
  {
    image_ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_ci.imageType = VkImageType(image_type);
    image_ci.extent = VkExtent3D(width, height, depth);
    image_ci.mipLevels = levels;
    image_ci.arrayLayers = layers;
    image_ci.format = VkFormat(format);
    image_ci.tiling = VkImageTiling(tiling);
    image_ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_ci.usage = VkImageUsageFlags(usage);
    image_ci.samples = VkSampleCountFlagBits(samples);
    image_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  return image_ci;
}

VkSamplerCreateInfo CreateSamplerCreateInfo(Filter min, Filter mag, const SamplerAddress &uvw) {
  VkSamplerCreateInfo sampler_ci{};
  {
    sampler_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_ci.magFilter = VkFilter(mag);
    sampler_ci.minFilter = VkFilter(min);
    sampler_ci.addressModeU = VkSamplerAddressMode(uvw.u_);
    sampler_ci.addressModeV = VkSamplerAddressMode(uvw.v_);
    sampler_ci.addressModeW = VkSamplerAddressMode(uvw.w_);
    sampler_ci.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_ci.compareEnable = VK_FALSE;
    sampler_ci.unnormalizedCoordinates = VK_FALSE;
    sampler_ci.compareOp = VK_COMPARE_OP_NEVER;
    sampler_ci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_ci.mipLodBias = 0.0f;
    sampler_ci.minLod = -1000.0f;
    sampler_ci.maxLod = 1000.0f;
    sampler_ci.maxAnisotropy = 1.0f;
  }
  return sampler_ci;
}

VkPipelineColorBlendAttachmentState CreateColorBlendAttachmentState(bool enable, BlendFactor from_color_factor, BlendFactor to_color_factor,
                                                                    BlendFactor from_alpha_factor, BlendFactor to_alpha_factor,
                                                                    BlendOperation color_operation, BlendOperation alpha_operation,
                                                                    ColorComponent color_write_mask) {
  VkPipelineColorBlendAttachmentState blend_attachment_state{};
  {
    blend_attachment_state.blendEnable = enable;
    blend_attachment_state.srcColorBlendFactor = VkBlendFactor(from_color_factor);
    blend_attachment_state.dstColorBlendFactor = VkBlendFactor(to_color_factor);
    blend_attachment_state.colorBlendOp = VkBlendOp(color_operation);
    blend_attachment_state.srcAlphaBlendFactor = VkBlendFactor(from_alpha_factor);
    blend_attachment_state.dstAlphaBlendFactor = VkBlendFactor(to_alpha_factor);
    blend_attachment_state.alphaBlendOp = VkBlendOp(alpha_operation);
    blend_attachment_state.colorWriteMask = std::to_underlying(color_write_mask);
  }
  return blend_attachment_state;
}

VkRenderingAttachmentInfo CreateRenderingAttachmentInfo(const VkImageView image_view, LoadOperation load_operation,
                                                        StoreOperation store_operation, ImageLayout image_layout) {
  VkRenderingAttachmentInfo rendering_ai{};
  {
    rendering_ai.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    rendering_ai.imageView = image_view;
    rendering_ai.imageLayout = VkImageLayout(image_layout);
    rendering_ai.loadOp = VkAttachmentLoadOp(load_operation);
    rendering_ai.storeOp = VkAttachmentStoreOp(store_operation);
    rendering_ai.clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};
  }

  return rendering_ai;
}

AccessMask GetAccessMask(ImageLayout layout) {
  switch (layout) {
  case ImageLayout::UNDEFINED:
  case ImageLayout::PRESENT_SRC_KHR:
    return AccessMask::NONE;
  case ImageLayout::PREINITIALIZED:
    return AccessMask::HOST_WRITE_BIT;
  case ImageLayout::COLOR_ATTACHMENT_OPTIMAL:
    return AccessMask::COLOR_ATTACHMENT_WRITE_BIT;
  case ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
    return AccessMask::DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  case ImageLayout::TRANSFER_SRC_OPTIMAL:
    return AccessMask::TRANSFER_READ_BIT;
  case ImageLayout::TRANSFER_DST_OPTIMAL:
    return AccessMask::TRANSFER_WRITE_BIT;
  case ImageLayout::SHADER_READ_ONLY_OPTIMAL:
    return AccessMask::SHADER_READ_BIT;
  default:
    break;
  }
}

} // namespace Innsmouth