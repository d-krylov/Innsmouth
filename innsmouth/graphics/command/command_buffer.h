#ifndef INNSMOUTH_COMMAND_BUFFER_H
#define INNSMOUTH_COMMAND_BUFFER_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"
#include <optional>
#include <span>

namespace Innsmouth {

class CommandBuffer {
public:
  CommandBuffer(const VkCommandPool command_pool);

  ~CommandBuffer();

  void Begin(VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
  void Reset();
  void End();

  void Submit();

  void BeginRendering();

  const VkCommandBuffer *get() const {
    return &command_buffer_;
  }

  void CommandBeginRendering(const Extent2D &extent, std::span<const RenderingAttachmentInfo> colors,
                             const std::optional<RenderingAttachmentInfo> &depth = std::nullopt,
                             const std::optional<RenderingAttachmentInfo> &stencil = std::nullopt);

  void CommandEndRendering();

  // OPTIONS
  void CommandSetViewport(float x, float y, float w, float h, float min_depth = 0.0f, float max_depth = 1.0f);
  void CommandSetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height);
  void CommandSetCullMode(CullModeMask mode);
  void CommandSetFrontFace(FrontFace front_face);
  void CommandEnableDepthTest(bool enabled);
  void CommandEnableDepthWrite(bool enabled);
  void CommandEnableStencilTest(bool enabled);

  // DRAW
  void CommandDraw(uint32_t vertex_count, uint32_t instance_count = 1, uint32_t first_vertex = 0, uint32_t first_instance = 0);
  void CommandDrawIndexed(uint32_t index_count, uint32_t instance_count = 1, uint32_t first_index = 0, int32_t vertex_offset = 0,
                          uint32_t first_instance = 0);

  void CommandDrawIndirect(const VkBuffer buffer, uint64_t offset, uint32_t draw_count, uint32_t stride);

  // BIND
  void CommandBindGraphicsPipeline(const VkPipeline graphics_pipeline);
  void CommandBindComputePipeline(const VkPipeline graphics_pipeline);
  void CommandBindVertexBuffer(const VkBuffer buffer, std::size_t offset);
  void CommandBindIndexBuffer(const VkBuffer buffer, std::size_t offset, VkIndexType index_type = VkIndexType::VK_INDEX_TYPE_UINT32);

  // BARRIER
  void CommandPipelineBarrier(std::span<const ImageMemoryBarrier2> image_barriers, std::span<const BufferMemoryBarrier2> buffer_barriers);
  void CommandImageMemoryBarrier(const VkImage &image, ImageLayout from_layout, ImageLayout to_layout, PipelineStageMask2 from_stage,
                                 PipelineStageMask2 to_stage, const ImageSubresourceRange &range);

  void CommandCopyBufferToImage(const VkBuffer buffer, const VkImage image, const VkExtent3D &extent);

  // PUSH
  void CommandPushDescriptorSet(VkPipelineLayout layout, uint32_t set_number, uint32_t binding, VkBuffer buffer);
  void CommandPushDescriptorSet(const VkPipelineLayout layout, uint32_t set_number, uint32_t binding, const VkImageView image_view,
                                const VkSampler sampler);

  template <typename T> void CommandPushConstants(const VkPipelineLayout layout, ShaderStageMask stage, const T &data, uint32_t offset = 0);

private:
  VkCommandBuffer command_buffer_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#include "command_buffer.ipp"

#endif // INNSMOUTH_COMMAND_BUFFER_H