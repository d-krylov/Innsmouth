#include "command_buffer.h"
#include "command_pool.h"
#include "innsmouth/graphics/synchronization/fence.h"

namespace Innsmouth {

CommandBuffer::CommandBuffer(const VkCommandPool command_pool) : command_pool_(command_pool) {
  command_buffer_ = AllocateCommandBuffer(command_pool_);
}

CommandBuffer::CommandBuffer(uint32_t family_index) {
  command_pool_ = CommandPool::CreateCommandPool(family_index, CommandPoolCreateMaskBits::E_RESET_COMMAND_BUFFER_BIT);
  command_buffer_ = AllocateCommandBuffer(command_pool_);
}

CommandBuffer::CommandBuffer(CommandBuffer &&other) noexcept {
  command_buffer_ = std::exchange(other.command_buffer_, VK_NULL_HANDLE);
  command_pool_ = std::exchange(other.command_pool_, VK_NULL_HANDLE);
  destroy_pool_ = std::exchange(other.destroy_pool_, false);
}

CommandBuffer &CommandBuffer::operator=(CommandBuffer &&other) noexcept {
  std::swap(command_buffer_, other.command_buffer_);
  std::swap(command_pool_, other.command_pool_);
  std::swap(destroy_pool_, other.destroy_pool_);
  return *this;
}

CommandBuffer::~CommandBuffer() {
  if (destroy_pool_) {
    vkDestroyCommandPool(GraphicsContext::Get()->GetDevice(), command_pool_, nullptr);
  }
}

VkCommandBuffer CommandBuffer::AllocateCommandBuffer(VkCommandPool command_pool) {
  VkCommandBuffer command_buffer = VK_NULL_HANDLE;
  CommandBufferAllocateInfo command_buffer_ai;
  command_buffer_ai.commandPool = command_pool;
  command_buffer_ai.level = CommandBufferLevel::E_PRIMARY;
  command_buffer_ai.commandBufferCount = 1;
  VK_CHECK(vkAllocateCommandBuffers(GraphicsContext::Get()->GetDevice(), command_buffer_ai, &command_buffer));
  return command_buffer;
}

const VkCommandBuffer *CommandBuffer::get() const {
  return &command_buffer_;
}

void CommandBuffer::Submit() {
  SubmitInfo submit_info;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer_;
  Fence fence;
  VK_CHECK(vkQueueSubmit(GraphicsContext::Get()->GetGraphicsQueue(), 1, submit_info, fence.GetHandle()));
  fence.Wait();
}

void CommandBuffer::Begin(CommandBufferUsageMask usage) {
  CommandBufferBeginInfo command_buffer_bi;
  command_buffer_bi.flags = usage;
  VK_CHECK(vkBeginCommandBuffer(command_buffer_, command_buffer_bi));
}

void CommandBuffer::End() {
  VK_CHECK(vkEndCommandBuffer(command_buffer_));
}

void CommandBuffer::Reset() {
  vkResetCommandBuffer(command_buffer_, 0);
}

void CommandBuffer::CommandBeginRendering(const Extent2D &extent, std::span<const RenderingAttachmentInfo> colors,
                                          const std::optional<RenderingAttachmentInfo> &depth,
                                          const std::optional<RenderingAttachmentInfo> &stencil) {
  RenderingInfo rendering_info;

  rendering_info.renderArea.offset = {0, 0};
  rendering_info.renderArea.extent = extent;
  rendering_info.layerCount = 1;
  rendering_info.colorAttachmentCount = colors.size();
  rendering_info.pColorAttachments = colors.data();
  rendering_info.pDepthAttachment = depth.has_value() ? &depth.value() : nullptr;
  rendering_info.pStencilAttachment = stencil.has_value() ? &stencil.value() : nullptr;

  vkCmdBeginRendering(command_buffer_, rendering_info);
}

void CommandBuffer::CommandEndRendering() {
  vkCmdEndRenderingKHR(command_buffer_);
}

// OPTIONS

void CommandBuffer::CommandSetViewport(float x, float y, float w, float h, float min_depth, float max_depth) {
  Viewport viewport;
  viewport.x = x;
  viewport.y = y;
  viewport.width = w;
  viewport.height = h;
  viewport.minDepth = min_depth;
  viewport.maxDepth = max_depth;
  vkCmdSetViewport(command_buffer_, 0, 1, viewport);
}

void CommandBuffer::CommandSetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height) {
  Rect2D scissor;
  scissor.offset.x = x;
  scissor.offset.y = y;
  scissor.extent.width = width;
  scissor.extent.height = height;
  vkCmdSetScissor(command_buffer_, 0, 1, scissor);
}

void CommandBuffer::CommandSetCullMode(CullModeMask mode) {
  vkCmdSetCullMode(command_buffer_, mode.GetValue());
}

void CommandBuffer::CommandSetFrontFace(FrontFace front_face) {
  vkCmdSetFrontFace(command_buffer_, static_cast<VkFrontFace>(front_face));
}

void CommandBuffer::CommandEnableDepthTest(bool enabled) {
  vkCmdSetDepthTestEnable(command_buffer_, enabled);
}

void CommandBuffer::CommandEnableDepthWrite(bool enabled) {
  vkCmdSetDepthWriteEnable(command_buffer_, enabled);
}

void CommandBuffer::CommandEnableStencilTest(bool enabled) {
  vkCmdSetStencilTestEnable(command_buffer_, enabled);
}

void CommandBuffer::CommandPipelineBarrier(std::span<const ImageMemoryBarrier2> image_barriers,
                                           std::span<const BufferMemoryBarrier2> buffer_barriers,
                                           std::span<const MemoryBarrier2> memory_barriers) {
  DependencyInfo dependency_info{};

  dependency_info.dependencyFlags = {};
  dependency_info.bufferMemoryBarrierCount = buffer_barriers.size();
  dependency_info.pBufferMemoryBarriers = buffer_barriers.data();
  dependency_info.imageMemoryBarrierCount = image_barriers.size();
  dependency_info.pImageMemoryBarriers = image_barriers.data();
  dependency_info.memoryBarrierCount = memory_barriers.size();
  dependency_info.pMemoryBarriers = memory_barriers.data();

  vkCmdPipelineBarrier2(command_buffer_, dependency_info);
}

void CommandBuffer::CommandMemoryBarrier(PipelineStageMask2 source_stage, AccessMask2 source_access, PipelineStageMask2 destination_stage,
                                         AccessMask2 destination_access) {
  std::array<MemoryBarrier2, 1> memory_barrier;
  memory_barrier[0].srcStageMask = source_stage;
  memory_barrier[0].srcAccessMask = source_access;
  memory_barrier[0].dstStageMask = destination_stage;
  memory_barrier[0].dstAccessMask = destination_access;

  CommandPipelineBarrier({}, {}, memory_barrier);
}

void CommandBuffer::CommandImageMemoryBarrier(VkImage image, ImageLayout source_layout, ImageLayout destination_layout,
                                              PipelineStageMask2 source_stage, PipelineStageMask2 destination_stage, AccessMask2 source_access,
                                              AccessMask2 destination_access, const ImageSubresourceRange &subresource) {

  std::array<ImageMemoryBarrier2, 1> image_memory_barrier;

  image_memory_barrier[0].srcStageMask = source_stage;
  image_memory_barrier[0].srcAccessMask = source_access;
  image_memory_barrier[0].dstStageMask = destination_stage;
  image_memory_barrier[0].dstAccessMask = destination_access;
  image_memory_barrier[0].oldLayout = source_layout;
  image_memory_barrier[0].newLayout = destination_layout;
  image_memory_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier[0].image = image;
  image_memory_barrier[0].subresourceRange = subresource;

  CommandPipelineBarrier(image_memory_barrier, {}, {});
}

void CommandBuffer::CommandBufferMemoryBarrier(VkBuffer buffer, PipelineStageMask2 source_stage, AccessMask2 source_access, //
                                               PipelineStageMask2 destination_stage, AccessMask2 destination_access) {
  std::array<BufferMemoryBarrier2, 1> buffer_memory_barrier;
  buffer_memory_barrier[0].buffer = buffer;
  buffer_memory_barrier[0].srcStageMask = source_stage;
  buffer_memory_barrier[0].srcAccessMask = source_access;
  buffer_memory_barrier[0].dstStageMask = destination_stage;
  buffer_memory_barrier[0].dstAccessMask = destination_access;
  buffer_memory_barrier[0].offset = 0;
  buffer_memory_barrier[0].size = VK_WHOLE_SIZE;
  buffer_memory_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  buffer_memory_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

  CommandPipelineBarrier({}, buffer_memory_barrier, {});
}

// BIND

void CommandBuffer::CommandBindPipeline(VkPipeline pipeline, PipelineBindPoint bind_point) {
  vkCmdBindPipeline(command_buffer_, VkPipelineBindPoint(bind_point), pipeline);
}

void CommandBuffer::CommandBindVertexBuffer(const VkBuffer buffer, std::size_t offset) {
  vkCmdBindVertexBuffers(command_buffer_, 0, 1, &buffer, &offset);
}

void CommandBuffer::CommandBindIndexBuffer(const VkBuffer buffer, std::size_t offset, VkIndexType index_type) {
  vkCmdBindIndexBuffer(command_buffer_, buffer, offset, index_type);
}

void CommandBuffer::CommandBindDescriptorSet(VkPipelineLayout pipeline_layout, VkDescriptorSet descriptor_set, uint32_t set) {
  vkCmdBindDescriptorSets(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, set, 1, &descriptor_set, 0, nullptr);
}

// DRAW
void CommandBuffer::CommandDraw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
  vkCmdDraw(command_buffer_, vertex_count, instance_count, first_vertex, first_instance);
}

void CommandBuffer::CommandDrawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset,
                                       uint32_t first_instance) {
  vkCmdDrawIndexed(command_buffer_, index_count, instance_count, first_index, vertex_offset, first_instance);
}

void CommandBuffer::CommandDrawIndexedIndirect(VkBuffer buffer, std::size_t byte_offset, uint32_t draw_count, uint32_t stride) {
  vkCmdDrawIndexedIndirect(command_buffer_, buffer, byte_offset, draw_count, stride);
}

void CommandBuffer::CommandDrawIndirect(const VkBuffer buffer, uint64_t offset, uint32_t draw_count, uint32_t stride) {
  vkCmdDrawIndirect(command_buffer_, buffer, offset, draw_count, stride);
}

// PUSH DESCRIPTORS
void CommandBuffer::CommandPushDescriptorSet(std::span<const DescriptorImageInfo> images, VkPipelineLayout layout, uint32_t set_number,
                                             uint32_t binding, DescriptorType descriptor_type, PipelineBindPoint bind_point) {
  WriteDescriptorSet write_descriptor_set;
  write_descriptor_set.dstBinding = binding;
  write_descriptor_set.dstArrayElement = 0;
  write_descriptor_set.descriptorType = descriptor_type;
  write_descriptor_set.descriptorCount = images.size();
  write_descriptor_set.pImageInfo = images.data();

  vkCmdPushDescriptorSetKHR(command_buffer_, VkPipelineBindPoint(bind_point), layout, set_number, 1, write_descriptor_set);
}

void CommandBuffer::CommandPushDescriptorSet(VkPipelineLayout layout, uint32_t set_number, uint32_t binding, VkBuffer buffer,
                                             PipelineBindPoint bind_point) {
  DescriptorBufferInfo descriptor_bi;
  descriptor_bi.buffer = buffer;
  descriptor_bi.offset = 0;
  descriptor_bi.range = VK_WHOLE_SIZE;

  WriteDescriptorSet write_descriptor_set;
  write_descriptor_set.dstBinding = binding;
  write_descriptor_set.dstArrayElement = 0;
  write_descriptor_set.descriptorType = DescriptorType::E_STORAGE_BUFFER;
  write_descriptor_set.descriptorCount = 1;
  write_descriptor_set.pBufferInfo = &descriptor_bi;

  vkCmdPushDescriptorSetKHR(command_buffer_, VkPipelineBindPoint(bind_point), layout, set_number, 1, write_descriptor_set);
}

void CommandBuffer::CommandPushDescriptorSet(VkPipelineLayout layout, uint32_t set, uint32_t binding,
                                             const VkAccelerationStructureKHR &acceleration, PipelineBindPoint bind_point) {
  WriteDescriptorSetAccelerationStructureKHR descriptor_ai{};
  descriptor_ai.accelerationStructureCount = 1;
  descriptor_ai.pAccelerationStructures = &acceleration;

  WriteDescriptorSet write_descriptor_set;
  write_descriptor_set.dstBinding = binding;
  write_descriptor_set.descriptorType = DescriptorType::E_ACCELERATION_STRUCTURE_KHR;
  write_descriptor_set.descriptorCount = 1;
  write_descriptor_set.pNext = &descriptor_ai;

  vkCmdPushDescriptorSetKHR(command_buffer_, VkPipelineBindPoint(bind_point), layout, set, 1, write_descriptor_set);
}

void CommandBuffer::CommandCopyBufferToImage(VkBuffer buffer, VkImage image, const Extent3D &extent) {
  ImageSubresourceLayers subresource_layers;

  subresource_layers.aspectMask = ImageAspectMaskBits::E_COLOR_BIT;
  subresource_layers.mipLevel = 0;
  subresource_layers.baseArrayLayer = 0;
  subresource_layers.layerCount = 1;

  BufferImageCopy buffer_image_copy;

  buffer_image_copy.bufferOffset = 0;
  buffer_image_copy.bufferRowLength = 0;
  buffer_image_copy.bufferImageHeight = 0;
  buffer_image_copy.imageSubresource = subresource_layers;
  buffer_image_copy.imageOffset = Offset3D(0, 0, 0);
  buffer_image_copy.imageExtent = extent;

  vkCmdCopyBufferToImage(command_buffer_, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, buffer_image_copy);
}

void CommandBuffer::CommandCopyBuffer(VkBuffer source, VkBuffer destination, std::size_t from_offset, std::size_t to_offset, std::size_t size) {
  BufferCopy buffer_copy;
  buffer_copy.srcOffset = from_offset;
  buffer_copy.dstOffset = to_offset;
  buffer_copy.size = size;
  vkCmdCopyBuffer(command_buffer_, source, destination, 1, buffer_copy);
}

void CommandBuffer::CommandBuildAccelerationStructure(std::span<const AccelerationStructureBuildGeometryInfoKHR> build_geometry_infos,
                                                      std::span<const AccelerationStructureBuildRangeInfoKHR *> build_range_infos) {
  auto primitive_count = build_range_infos.size();
  auto geometry_infos = reinterpret_cast<const VkAccelerationStructureBuildGeometryInfoKHR *>(build_geometry_infos.data());
  auto range_infos = reinterpret_cast<const VkAccelerationStructureBuildRangeInfoKHR **>(build_range_infos.data());
  vkCmdBuildAccelerationStructuresKHR(command_buffer_, primitive_count, geometry_infos, range_infos);
}

void CommandBuffer::CommandBlitImage(VkImage source_image, ImageLayout source_layout) {
  // vkCmdBlitImage(command_buffer_, source_image, source_layout,  );
}

void CommandBuffer::CommandTraceRay(const StridedDeviceAddressRegionKHR &raygen, const StridedDeviceAddressRegionKHR &miss,
                                    const StridedDeviceAddressRegionKHR &hit, uint32_t width, uint32_t height, uint32_t depth) {
  StridedDeviceAddressRegionKHR callable;
  vkCmdTraceRaysKHR(command_buffer_, raygen, miss, hit, callable, width, height, depth);
}

} // namespace Innsmouth