#include "structure_tools.h"

namespace Innsmouth {

ImageSubresourceRange GetImageSubresourceRange(ImageAspectMask aspect, uint32_t base_level, uint32_t level_count, uint32_t base_layer,
                                               uint32_t layer_count) {
  ImageSubresourceRange image_subresource_range;
  {
    image_subresource_range.aspectMask = aspect;
    image_subresource_range.baseMipLevel = base_level;
    image_subresource_range.levelCount = level_count;
    image_subresource_range.baseArrayLayer = base_layer;
    image_subresource_range.layerCount = layer_count;
  }
  return image_subresource_range;
}

PipelineColorBlendAttachmentState GetColorBlendAttachmentState(bool enable, BlendFactor from_color, BlendFactor to_color,
                                                               BlendOp color_operation, BlendFactor from_alpha, BlendFactor to_alpha,
                                                               BlendOp alpha_operation) {
  PipelineColorBlendAttachmentState color_blend_attachment_state;
  {
    color_blend_attachment_state.blendEnable = enable;
    color_blend_attachment_state.srcColorBlendFactor = from_color;
    color_blend_attachment_state.dstColorBlendFactor = to_color;
    color_blend_attachment_state.colorBlendOp = color_operation;
    color_blend_attachment_state.srcAlphaBlendFactor = from_alpha;
    color_blend_attachment_state.dstAlphaBlendFactor = to_alpha;
    color_blend_attachment_state.alphaBlendOp = alpha_operation;
    color_blend_attachment_state.colorWriteMask =
      ColorComponentMaskBits::E_R_BIT | ColorComponentMaskBits::E_G_BIT | ColorComponentMaskBits::E_B_BIT | ColorComponentMaskBits::E_A_BIT;
  }
  return color_blend_attachment_state;
}

} // namespace Innsmouth