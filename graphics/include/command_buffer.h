#ifndef INNSMOUTH_COMMAND_BUFFER_H
#define INNSMOUTH_COMMAND_BUFFER_H

#include "graphics/include/graphics_types.h"
#include <span>

namespace Innsmouth {

class GraphicsPipeline;
class Buffer;
class Image;

class CommandBuffer {
public:
  CommandBuffer(const VkCommandPool command_pool, bool begin = false);

  ~CommandBuffer();

  NO_COPY_SEMANTIC(CommandBuffer);

  CommandBuffer(CommandBuffer &&other) noexcept;

  operator const VkCommandBuffer &() const { return command_buffer_; }

  [[nodiscard]] const VkCommandBuffer *get() const { return &command_buffer_; }

  void Begin(CommandBufferUsage usage = CommandBufferUsage::ONE_TIME_SUBMIT);

  void End();

  void Reset();

  void Flush();

  void CommandSetViewport(float x, float y, float w, float h);
  void CommandSetScissor(const VkRect2D &scissor);
  void CommandSetCullMode(bool front, bool back);
  void CommandSetFrontFace(FrontFace front_face);
  void CommandEnableDepthTest(bool b);
  void CommandEnableStencilTest(bool b);
  void CommandEnableBlend(bool b);

  void CommandBeginRendering(const VkImageView image_view, const VkExtent2D &extent);
  void CommandEndRendering();
  void CommandBindPipeline(const GraphicsPipeline &graphics_pipeline);
  void CommandBindVertexBuffer(const Buffer &buffer, VkDeviceSize offset = 0);
  void CommandBindIndexBuffer(const Buffer &buffer, VkDeviceSize offset = 0,
                              VkIndexType index_type = VkIndexType::VK_INDEX_TYPE_UINT32);

  void CommandDraw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
                   uint32_t first_instance);

  void CommandDrawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index,
                          int32_t vertex_offset, uint32_t first_instance);

  void CommandPushConstants(const GraphicsPipeline &graphics_pipeline, ShaderStage stage,
                            std::span<const std::byte> data, uint32_t offset = 0);

  void CommandPushDescriptorSet(const GraphicsPipeline &graphics_pipeline, uint32_t set_number,
                                std::span<const VkWriteDescriptorSet> sets);

  void CommandCopyBufferToImage(const Buffer &buffer, const Image &image, const VkExtent3D &extent,
                                uint32_t level = 0, uint32_t base_layer = 0, uint32_t layers = 1,
                                VkDeviceSize buffer_offset = 0,
                                const VkOffset3D &image_offset = {0, 0, 0});

  void CommandSetPrimitiveTopology(PrimitiveTopology topology);
  void CommandSetColorBlendEquation();

  void CommandSetImageLayout(const VkImage &image, ImageLayout from, ImageLayout to,
                             const VkImageSubresourceRange &range,
                             PipelineStage source_stage = PipelineStage::ALL_COMMANDS,
                             PipelineStage destination_stage = PipelineStage::ALL_COMMANDS);

private:
  VkCommandBuffer command_buffer_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_COMMAND_BUFFER_H
