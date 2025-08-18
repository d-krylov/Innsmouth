#include "command_buffer.h"

namespace Innsmouth {

CommandBuffer::CommandBuffer(const VkCommandPool command_pool) {
  VkCommandBufferAllocateInfo command_buffer_ai{};
  {
    command_buffer_ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_ai.commandPool = command_pool;
    command_buffer_ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_ai.commandBufferCount = 1;
  }
  VK_CHECK(vkAllocateCommandBuffers(GraphicsContext::Get().GetDevice(), &command_buffer_ai, &command_buffer_));
}

CommandBuffer::~CommandBuffer() {
}

void CommandBuffer::Begin(VkCommandBufferUsageFlags usage) {
  VkCommandBufferBeginInfo command_buffer_bi{};
  {
    command_buffer_bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_bi.flags = usage;
  }
  VK_CHECK(vkBeginCommandBuffer(command_buffer_, &command_buffer_bi));
}

void CommandBuffer::End() {
  VK_CHECK(vkEndCommandBuffer(command_buffer_));
}

void CommandBuffer::Reset() {
  vkResetCommandBuffer(command_buffer_, 0);
}

void CommandBuffer::CommandBeginRendering(const VkExtent2D &extent, std::span<const VkRenderingAttachmentInfo> colors,
                                          const std::optional<VkRenderingAttachmentInfo> &depth,
                                          const std::optional<VkRenderingAttachmentInfo> &stencil) {
  VkRenderingInfo rendering_info{};
  {
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    rendering_info.renderArea.offset = {0, 0};
    rendering_info.renderArea.extent = extent;
    rendering_info.layerCount = 1;
    rendering_info.colorAttachmentCount = colors.size();
    rendering_info.pColorAttachments = colors.data();
    rendering_info.pDepthAttachment = depth.has_value() ? &depth.value() : nullptr;
    rendering_info.pStencilAttachment = stencil.has_value() ? &stencil.value() : nullptr;
  }

  vkCmdBeginRendering(command_buffer_, &rendering_info);
}

void CommandBuffer::CommandEndRendering() {
  vkCmdEndRenderingKHR(command_buffer_);
}

// OPTIONS

void CommandBuffer::CommandSetViewport(float x, float y, float w, float h, float min_depth, float max_depth) {
  VkViewport viewport{};
  {
    viewport.x = x;
    viewport.y = y;
    viewport.width = w;
    viewport.height = h;
    viewport.minDepth = min_depth;
    viewport.maxDepth = max_depth;
  }
  vkCmdSetViewport(command_buffer_, 0, 1, &viewport);
}

void CommandBuffer::CommandSetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height) {
  VkRect2D scissor;
  {
    scissor.offset.x = x;
    scissor.offset.y = y;
    scissor.extent.width = width;
    scissor.extent.height = height;
  }
  vkCmdSetScissor(command_buffer_, 0, 1, &scissor);
}

void CommandBuffer::CommandSetCullMode(VkCullModeFlags mode) {
  vkCmdSetCullMode(command_buffer_, mode);
}

void CommandBuffer::CommandSetFrontFace(VkFrontFace front_face) {
  vkCmdSetFrontFace(command_buffer_, front_face);
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

void CommandBuffer::CommandPipelineBarrier(std::span<const VkImageMemoryBarrier2> image_barriers,
                                           std::span<const VkBufferMemoryBarrier2> buffer_barriers) {
  VkDependencyInfo dependency_info{};
  {
    dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency_info.dependencyFlags = 0;
    dependency_info.bufferMemoryBarrierCount = buffer_barriers.size();
    dependency_info.pBufferMemoryBarriers = buffer_barriers.data();
    dependency_info.imageMemoryBarrierCount = image_barriers.size();
    dependency_info.pImageMemoryBarriers = image_barriers.data();
  }

  vkCmdPipelineBarrier2(command_buffer_, &dependency_info);
}

void CommandBuffer::CommandImageMemoryBarrier(const VkImage &image, VkImageLayout from_layout, VkImageLayout to_layout,
                                              VkPipelineStageFlags from_stage, VkPipelineStageFlags to_stage,
                                              const VkImageSubresourceRange &range) {

  auto from_access_mask = GetAccessMask(from_layout);
  auto to_access_mask = GetAccessMask(to_layout);

  std::array<VkImageMemoryBarrier2, 1> image_memory_barrier = {};
  {
    image_memory_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    image_memory_barrier[0].srcStageMask = from_stage;
    image_memory_barrier[0].srcAccessMask = from_access_mask;
    image_memory_barrier[0].dstStageMask = to_stage;
    image_memory_barrier[0].dstAccessMask = to_access_mask;
    image_memory_barrier[0].oldLayout = from_layout;
    image_memory_barrier[0].newLayout = to_layout;
    image_memory_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier[0].image = image;
    image_memory_barrier[0].subresourceRange = range;
  }

  std::array<VkBufferMemoryBarrier2, 0> buffer_memory_barrier{};

  CommandPipelineBarrier(image_memory_barrier, buffer_memory_barrier);
}

void CommandBuffer::CommandBindGraphicsPipeline(const VkPipeline graphics_pipeline) {
  vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);
}

void CommandBuffer::CommandBindComputePipeline(const VkPipeline graphics_pipeline) {
  vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_COMPUTE, graphics_pipeline);
}

// DRAW
void CommandBuffer::CommandDraw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
  vkCmdDraw(command_buffer_, vertex_count, instance_count, first_vertex, first_instance);
}

void CommandBuffer::CommandDrawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset,
                                       uint32_t first_instance) {
  vkCmdDrawIndexed(command_buffer_, index_count, instance_count, first_index, vertex_offset, first_instance);
}

void CommandBuffer::CommandDrawIndirect(const VkBuffer buffer, uint64_t offset, uint32_t draw_count, uint32_t stride) {
  vkCmdDrawIndirect(command_buffer_, buffer, offset, draw_count, stride);
}

} // namespace Innsmouth