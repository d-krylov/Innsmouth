#ifndef INNSMOUTH_COMMAND_BUFFER_H
#define INNSMOUTH_COMMAND_BUFFER_H

#include "core/include/concepts.h"
#include "graphics/pipeline/graphics_pipeline.h"
#include <span>

namespace Innsmouth {

class GraphicsPipeline;
class Buffer;
class Image;

class CommandBuffer {
public:
  CommandBuffer(const VkCommandPool command_pool, bool begin = false);

  NO_COPY_SEMANTIC(CommandBuffer);

  CommandBuffer(CommandBuffer &&other) noexcept;

  operator const VkCommandBuffer &() const { return command_buffer_; }

  [[nodiscard]] const VkCommandBuffer *get() const { return &command_buffer_; }

  void Begin(CommandBufferUsage usage = CommandBufferUsage::ONE_TIME_SUBMIT);

  void End();

  void Reset();

  void Flush();

  void CommandSetViewport(float x, float y, float w, float h);
  void CommandSetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height);
  void CommandSetCullMode(CullMode mode);
  void CommandSetFrontFace(FrontFace front_face);

  // Depth
  void CommandEnableDepthTest(bool b);
  void CommandEnableDepthWrite(bool b);
  void CommandEnableStencilTest(bool b);
  void CommandDepthCompareOperation(CompareOperation compare_operation);
  void CommandDepthBounds(float min, float max);

  template <ContiguousSizedRange<RenderingAttachment> R>
  void CommandBeginRendering(const VkExtent2D &extent, R &&colors,
                             std::optional<RenderingAttachment> depth = std::nullopt,
                             std::optional<RenderingAttachment> stencil = std::nullopt);

  void CommandEndRendering();
  void CommandBindPipeline(const GraphicsPipeline &graphics_pipeline);
  void CommandBindVertexBuffer(const Buffer &buffer, VkDeviceSize offset = 0);
  void CommandBindIndexBuffer(const Buffer &buffer, VkDeviceSize offset = 0,
                              VkIndexType index_type = VkIndexType::VK_INDEX_TYPE_UINT32);

  void CommandDraw(uint32_t vertex_count, uint32_t instance_count, uint32_t vertex_0, uint32_t instance_0);

  void CommandDrawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t index_0,
                          int32_t vertex_offset, uint32_t instance_0);

  template <typename T>
  void CommandPushConstants(const GraphicsPipeline &graphics_pipeline, ShaderStage stage, const T &data,
                            uint32_t offset = 0);

  void CommandPushDescriptorSet(const GraphicsPipeline &graphics_pipeline, uint32_t set_number,
                                uint32_t binding, const Image &image);

  void CommandPushDescriptorSet(const GraphicsPipeline &graphics_pipeline, uint32_t set_number,
                                uint32_t binding, const Buffer &buffer, uint64_t offset, uint64_t size);

  void CommandCopyBufferToImage(const Buffer &buffer, const Image &image, const VkExtent3D &extent,
                                uint32_t level = 0, uint32_t base_layer = 0, uint32_t layers = 1,
                                VkDeviceSize buffer_offset = 0, const VkOffset3D &image_offset = {0, 0, 0});

  void CommandSetPrimitiveTopology(PrimitiveTopology topology);

  void CommandSetImageLayout(const VkImage &image, ImageLayout from, ImageLayout to,
                             const VkImageSubresourceRange &range,
                             PipelineStage source_stage = PipelineStage::ALL_COMMANDS,
                             PipelineStage destination_stage = PipelineStage::ALL_COMMANDS);

private:
  VkCommandBuffer command_buffer_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_COMMAND_BUFFER_H
