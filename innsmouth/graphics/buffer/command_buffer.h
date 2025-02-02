#ifndef INNSMOUTH_COMMAND_BUFFER_H
#define INNSMOUTH_COMMAND_BUFFER_H

#include "graphics/pipeline/graphics_pipeline.h"

namespace Innsmouth {

class Buffer;
class Image;

class CommandBuffer {
public:
  CommandBuffer(const VkCommandPool command_pool);
  ~CommandBuffer();

  void Begin(CommandBufferUsage usage = CommandBufferUsage::ONE_TIME_SUBMIT);
  void End();
  void Reset();
  void Submit();

  operator const VkCommandBuffer &() const { return command_buffer_; }

  const VkCommandBuffer *get() const { return &command_buffer_; }

  void CommandBeginRendering(const VkExtent2D &extent, std::span<const VkRenderingAttachmentInfo> colors,
                             const std::optional<VkRenderingAttachmentInfo> &depth = std::nullopt,
                             const std::optional<VkRenderingAttachmentInfo> &stencil = std::nullopt);

  void CommandEndRendering();

  // BIND
  void CommandBindPipeline(const GraphicsPipeline &graphics_pipeline);
  void CommandBindVertexBuffer(const Buffer &buffer, uint64_t offset = 0);
  void CommandBindIndexBuffer(const Buffer &buffer, uint64_t offset = 0, bool is_short_index = false);

  // OPTIONS
  void CommandSetViewport(float x, float y, float w, float h, float min_depth = 0.0f, float max_depth = 1.0f);
  void CommandSetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height);
  void CommandDepthBounds(float min, float max);
  void CommandEnableStencilTest(bool b);
  void CommandEnableDepthWrite(bool b);
  void CommandEnableDepthTest(bool b);
  void CommandSetCullMode(CullMode mode);
  void CommandSetFrontFace(FrontFace front_face);
  void CommandDepthCompareOperation(CompareOperation compare_operation);

  // DRAW
  void CommandDraw(uint32_t vertex_count, uint32_t instance_count = 1, uint32_t first_vertex = 0, uint32_t first_instance = 0);

  // BARRIER
  void CommandPipelineBarrier(std::span<const VkImageMemoryBarrier2> image_barriers,
                              std::span<const VkBufferMemoryBarrier2> buffer_barriers);

  void ImageMemoryBarrier(const VkImage &image, ImageLayout from_layout, ImageLayout to_layout, PipelineStage from_stage,
                          PipelineStage to_stage, const VkImageSubresourceRange &range);

  void CommandCopyBufferToImage(const Buffer &buffer, const Image &image, uint32_t mip_level, uint32_t base_layer, uint32_t layers,
                                uint64_t buffer_offset, int32_t image_offset_x, int32_t image_offset_y, int32_t image_offset_z);

  void CommandPushDescriptorSet(const GraphicsPipeline &graphics_pipeline, uint32_t set_number, uint32_t binding, const Buffer &buffer,
                                uint64_t offset, uint64_t size);

  template <typename T>
  void CommandPushConstants(const GraphicsPipeline &graphics_pipeline, ShaderStage stage, const T &data, uint32_t offset = 0);

private:
  VkCommandBuffer command_buffer_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#include "command_buffer.ipp"

#endif // INNSMOUTH_COMMAND_BUFFER_H
