#ifndef INNSMOUTH_STRUCTURE_TOOLS_H
#define INNSMOUTH_STRUCTURE_TOOLS_H

#include "graphics_types.h"

namespace Innsmouth {

AccessMask GetAccessMask(ImageLayout layout);

// clang-format off
VkImageSubresourceRange CreateImageSubresourceRange(
  ImageAspect aspect = ImageAspect::COLOR_BIT,
  uint32_t base_level = 0,
  uint32_t levels = 1,
  uint32_t base_layer = 0,
  uint32_t layers = 1);

VkPipelineColorBlendAttachmentState CreateColorBlendAttachmentState(
  bool enable,
  BlendFactor from_color_factor = BlendFactor::SRC_ALPHA,
  BlendFactor to_color_factor = BlendFactor::ONE_MINUS_SRC_ALPHA,
  BlendFactor from_alpha_factor = BlendFactor::ONE,
  BlendFactor to_alpha_factor = BlendFactor::ONE_MINUS_SRC_ALPHA,
  BlendOperation color_operation = BlendOperation::ADD,
  BlendOperation alpha_operation = BlendOperation::ADD,
  ColorComponent color_write_mask = ColorComponent::R | ColorComponent::G | ColorComponent::B | ColorComponent::A);

VkRenderingAttachmentInfo CreateRenderingAttachmentInfo(
  const VkImageView image_view,
  LoadOperation load_operation = LoadOperation::CLEAR,
  StoreOperation store_operation = StoreOperation::STORE,
  ImageLayout image_layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL);
// clang-format on

} // namespace Innsmouth

#endif // INNSMOUTH_STRUCTURE_TOOLS_H