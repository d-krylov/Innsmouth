#ifndef INNSMOUTH_COMMAND_BUFFER_H
#define INNSMOUTH_COMMAND_BUFFER_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"
#include <optional>
#include <span>

namespace Innsmouth {

class CommandBuffer {
public:
  CommandBuffer(uint32_t family_index);

  CommandBuffer(const VkCommandPool command_pool);

  ~CommandBuffer();

  CommandBuffer(const CommandBuffer &) = delete;
  CommandBuffer &operator=(const CommandBuffer &) = delete;

  CommandBuffer(CommandBuffer &&other) noexcept;
  CommandBuffer &operator=(CommandBuffer &&other) noexcept;

  static VkCommandBuffer AllocateCommandBuffer(VkCommandPool command_pool);

  void Begin(CommandBufferUsageMask usage = CommandBufferUsageMaskBits::E_SIMULTANEOUS_USE_BIT);
  void Reset();
  void End();

  void Submit();

  void BeginRendering();

  const VkCommandBuffer *get() const;

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

  void CommandDrawIndexedIndirect(VkBuffer buffer, std::size_t offset, uint32_t draw_count,
                                  uint32_t stride = sizeof(DrawIndexedIndirectCommand));

  void CommandDrawIndexed(uint32_t index_count, uint32_t instance_count = 1, uint32_t first_index = 0, int32_t vertex_offset = 0,
                          uint32_t first_instance = 0);

  void CommandDrawIndirect(const VkBuffer buffer, uint64_t offset, uint32_t draw_count, uint32_t stride);

  // BIND
  void CommandBindPipeline(VkPipeline pipeline, PipelineBindPoint bind_point);
  void CommandBindVertexBuffer(const VkBuffer buffer, std::size_t offset);
  void CommandBindIndexBuffer(const VkBuffer buffer, std::size_t offset, VkIndexType index_type = VkIndexType::VK_INDEX_TYPE_UINT32);
  void CommandBindDescriptorSet(VkPipelineLayout pipeline_layout, VkDescriptorSet descriptor_set, uint32_t set);

  // BARRIER
  void CommandMemoryBarrier(PipelineStageMask2 source_stage, AccessMask2 source_access, PipelineStageMask2 destination_stage,
                            AccessMask2 destination_access);

  void CommandImageMemoryBarrier(VkImage image, ImageLayout source_layout, ImageLayout destination_layout, PipelineStageMask2 source_stage,
                                 PipelineStageMask2 destination_stage, AccessMask2 source_access, AccessMask2 destination_access,
                                 const ImageSubresourceRange &subresource);

  void CommandBufferMemoryBarrier(VkBuffer buffer, PipelineStageMask2 source_stage, AccessMask2 source_access,
                                  PipelineStageMask2 destination_stage, AccessMask2 destination_access);

  void CommandPipelineBarrier(std::span<const ImageMemoryBarrier2> image_barriers, std::span<const BufferMemoryBarrier2> buffer_barriers,
                              std::span<const MemoryBarrier2> memory_barriers);

  // COPY
  void CommandCopyBufferToImage(VkBuffer buffer, VkImage image, const Extent3D &extent);
  void CommandCopyBuffer(VkBuffer source, VkBuffer destination, std::size_t from_offset, std::size_t to_offset, std::size_t size);

  // PUSH
  void CommandPushDescriptorSet(VkPipelineLayout layout, uint32_t set, uint32_t binding, const VkAccelerationStructureKHR &acceleration,
                                PipelineBindPoint bind_point);

  void CommandPushDescriptorSet(VkPipelineLayout layout, uint32_t set_number, uint32_t binding, VkBuffer buffer, PipelineBindPoint bind_point);

  void CommandPushDescriptorSet(std::span<const DescriptorImageInfo> images, const VkPipelineLayout layout, uint32_t set_number,
                                uint32_t binding, DescriptorType descriptor_type, PipelineBindPoint bind_point);

  template <typename T> void CommandPushConstants(const VkPipelineLayout layout, ShaderStageMask stage, const T &data, uint32_t offset = 0);

  void CommandBuildAccelerationStructure(std::span<const AccelerationStructureBuildGeometryInfoKHR> build_geometry_infos,
                                         std::span<const AccelerationStructureBuildRangeInfoKHR *> build_range_infos);

  void CommandBlitImage(VkImage source_image, ImageLayout source_layout);

  void CommandTraceRay(const StridedDeviceAddressRegionKHR &raygen, const StridedDeviceAddressRegionKHR &miss,
                       const StridedDeviceAddressRegionKHR &hit, uint32_t width, uint32_t height, uint32_t depth);

private:
  VkCommandBuffer command_buffer_{VK_NULL_HANDLE};
  VkCommandPool command_pool_{VK_NULL_HANDLE};
  bool destroy_pool_{false};
};

} // namespace Innsmouth

#include "command_buffer.ipp"

#endif // INNSMOUTH_COMMAND_BUFFER_H