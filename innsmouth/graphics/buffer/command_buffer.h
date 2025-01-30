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
  CommandBuffer();
  ~CommandBuffer();

  void Begin(CommandBufferUsage usage);
  void End();

  // BIND
  void CommandBindPipeline(const GraphicsPipeline &graphics_pipeline);
  void CommandBindVertexBuffer(const Buffer &buffer, uint64_t offset = 0);
  void CommandBindIndexBuffer(const Buffer &buffer, uint64_t offset = 0, bool is_short_index = false);

  // OPTIONS
  void CommandSetViewport(float x, float y, float w, float h, float min_depth = 0.0f, float max_depth = 1.0f);
  void CommandSetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height);

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

private:
  VkCommandBuffer command_buffer_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_COMMAND_BUFFER_H
