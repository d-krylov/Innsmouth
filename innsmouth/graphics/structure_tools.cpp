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

} // namespace Innsmouth