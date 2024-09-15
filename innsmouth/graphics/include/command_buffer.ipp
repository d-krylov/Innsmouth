#ifndef INNSMOUTH_COMMAND_BUFFER_IPP
#define INNSMOUTH_COMMAND_BUFFER_IPP

#include "command_buffer.h"

namespace Innsmouth {

void CommandBuffer::CommandBeginRendering(const VkExtent2D &extent, Range<RenderingAttachment> auto &&colors,
                                          std::optional<RenderingAttachment> depth,
                                          std::optional<RenderingAttachment> stencil) {

  std::vector<VkRenderingAttachmentInfo> rendering_ais;
  for (auto &attachment : colors) {
    VkRenderingAttachmentInfo rendering_ai{};
    {
      rendering_ai.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
      rendering_ai.imageView = attachment.image_view_;
      rendering_ai.imageLayout = VkImageLayout(ImageLayout::COLOR_ATTACHMENT_OPTIMAL);
      rendering_ai.loadOp = VkAttachmentLoadOp(attachment.load_operation_);
      rendering_ai.storeOp = VkAttachmentStoreOp(attachment.store_operation_);
      rendering_ai.clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};
    }
    rendering_ais.emplace_back(rendering_ai);
  }

  VkRenderingAttachmentInfo depth_ai{};

  if (depth.has_value()) {
    depth_ai.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depth_ai.imageView = depth.value().image_view_;
    depth_ai.imageLayout = VkImageLayout(ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    depth_ai.loadOp = VkAttachmentLoadOp(depth.value().load_operation_);
    depth_ai.storeOp = VkAttachmentStoreOp(depth.value().store_operation_);
    depth_ai.clearValue.depthStencil = {1.0f, 0};
  }

  VkRenderingInfo rendering_info{};
  {
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    rendering_info.renderArea.offset = {0, 0};
    rendering_info.renderArea.extent = extent;
    rendering_info.layerCount = 1;
    rendering_info.colorAttachmentCount = rendering_ais.size();
    rendering_info.pColorAttachments = rendering_ais.data();
    rendering_info.pDepthAttachment = depth.has_value() ? &depth_ai : nullptr;
    rendering_info.pStencilAttachment = depth.has_value() ? &depth_ai : nullptr;
  }

  vkCmdBeginRendering(command_buffer_, &rendering_info);
}

template <typename T>
void CommandBuffer::CommandPushConstants(const GraphicsPipeline &graphics_pipeline, ShaderStage stage,
                                         const T &data, uint32_t offset) {
  vkCmdPushConstants(command_buffer_, graphics_pipeline.GetPipelineLayout(), VkShaderStageFlags(stage),
                     offset, sizeof(T), &data);
}

} // namespace Innsmouth

#endif // INNSMOUTH_COMMAND_BUFFER_IPP