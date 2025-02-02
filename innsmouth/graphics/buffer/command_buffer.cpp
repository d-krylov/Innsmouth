#include "command_buffer.h"
#include "graphics/include/graphics.h"
#include "graphics/include/structure_tools.h"
#include "graphics/synchronization/fence.h"
#include "buffer.h"
#include <ranges>

namespace Innsmouth {

CommandBuffer::CommandBuffer(const VkCommandPool command_pool) {
  VkCommandBufferAllocateInfo command_buffer_ai{};
  {
    command_buffer_ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_ai.commandPool = command_pool;
    command_buffer_ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_ai.commandBufferCount = 1;
  }
  VK_CHECK(vkAllocateCommandBuffers(Device(), &command_buffer_ai, &command_buffer_));
}

CommandBuffer::~CommandBuffer() {}

void CommandBuffer::Begin(CommandBufferUsage usage) {
  VkCommandBufferBeginInfo command_buffer_bi{};
  {
    command_buffer_bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_bi.flags = std::to_underlying(usage);
  }
  VK_CHECK(vkBeginCommandBuffer(command_buffer_, &command_buffer_bi));
}

void CommandBuffer::End() { VK_CHECK(vkEndCommandBuffer(command_buffer_)); }

void CommandBuffer::Reset() { vkResetCommandBuffer(command_buffer_, 0); }

void CommandBuffer::CommandEndRendering() { vkCmdEndRenderingKHR(command_buffer_); }

void CommandBuffer::Submit() {
  VkSubmitInfo submit_info{};
  {
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer_;
  }

  Fence fence(false);

  VK_CHECK(vkQueueSubmit(GraphicsQueue(), 1, &submit_info, fence));

  fence.Wait();
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
    rendering_info.colorAttachmentCount = std::ranges::size(colors);
    rendering_info.pColorAttachments = std::ranges::data(colors);
    rendering_info.pDepthAttachment = depth.has_value() ? &depth.value() : nullptr;
    rendering_info.pStencilAttachment = stencil.has_value() ? &stencil.value() : nullptr;
  }

  vkCmdBeginRendering(command_buffer_, &rendering_info);
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

void CommandBuffer::CommandSetCullMode(CullMode mode) { vkCmdSetCullMode(command_buffer_, VkCullModeFlags(mode)); }

void CommandBuffer::CommandSetFrontFace(FrontFace front_face) { vkCmdSetFrontFace(command_buffer_, VkFrontFace(front_face)); }

void CommandBuffer::CommandEnableDepthTest(bool b) { vkCmdSetDepthTestEnable(command_buffer_, uint32_t(b)); }

void CommandBuffer::CommandEnableDepthWrite(bool b) { vkCmdSetDepthWriteEnable(command_buffer_, uint32_t(b)); }

void CommandBuffer::CommandEnableStencilTest(bool b) { vkCmdSetStencilTestEnable(command_buffer_, uint32_t(b)); }

void CommandBuffer::CommandDepthCompareOperation(CompareOperation compare_operation) {
  vkCmdSetDepthCompareOp(command_buffer_, VkCompareOp(compare_operation));
}

void CommandBuffer::CommandDepthBounds(float min, float max) { vkCmdSetDepthBounds(command_buffer_, min, max); }

// DRAW
void CommandBuffer::CommandDraw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
  vkCmdDraw(command_buffer_, vertex_count, instance_count, first_vertex, first_instance);
}

void CommandBuffer::CommandBindPipeline(const GraphicsPipeline &graphics_pipeline) {
  vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);
}

void CommandBuffer::CommandPushDescriptorSet(const GraphicsPipeline &graphics_pipeline, uint32_t set_number, uint32_t binding,
                                             const Buffer &buffer, uint64_t offset, uint64_t size) {

  VkDescriptorBufferInfo descriptor_bi{};
  {
    descriptor_bi.buffer = buffer.GetBuffer();
    descriptor_bi.offset = offset;
    descriptor_bi.range = size;
  }

  VkWriteDescriptorSet write_descriptor_set{};
  {
    write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_set.dstBinding = binding;
    write_descriptor_set.dstArrayElement = 0;
    write_descriptor_set.descriptorType = VkDescriptorType(DescriptorType::UNIFORM_BUFFER);
    write_descriptor_set.descriptorCount = 1;
    write_descriptor_set.pBufferInfo = &descriptor_bi;
  }

  vkCmdPushDescriptorSetKHR(command_buffer_, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline.GetPipelineLayout(),
                            set_number, 1, &write_descriptor_set);
}

void CommandBuffer::CommandPipelineBarrier(std::span<const VkImageMemoryBarrier2> image_barriers,
                                           std::span<const VkBufferMemoryBarrier2> buffer_barriers) {
  VkDependencyInfo dependency_info{};
  {
    dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency_info.dependencyFlags = 0;
    dependency_info.bufferMemoryBarrierCount = std::ranges::size(buffer_barriers);
    dependency_info.pBufferMemoryBarriers = std::ranges::data(buffer_barriers);
    dependency_info.imageMemoryBarrierCount = std::ranges::size(image_barriers);
    dependency_info.pImageMemoryBarriers = std::ranges::data(image_barriers);
  }

  vkCmdPipelineBarrier2(command_buffer_, &dependency_info);
}

void CommandBuffer::ImageMemoryBarrier(const VkImage &image, ImageLayout from_layout, ImageLayout to_layout, PipelineStage from_stage,
                                       PipelineStage to_stage, const VkImageSubresourceRange &range) {

  auto from_access_mask = GetAccessMask(from_layout);
  auto to_access_mask = GetAccessMask(to_layout);

  VkImageMemoryBarrier2 image_memory_barrier{};
  {
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    image_memory_barrier.srcStageMask = std::to_underlying(from_stage);
    image_memory_barrier.srcAccessMask = std::to_underlying(from_access_mask);
    image_memory_barrier.dstStageMask = std::to_underlying(to_stage);
    image_memory_barrier.dstAccessMask = std::to_underlying(to_access_mask);
    image_memory_barrier.oldLayout = VkImageLayout(from_layout);
    image_memory_barrier.newLayout = VkImageLayout(to_layout);
    image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.image = image;
    image_memory_barrier.subresourceRange = range;
  }

  std::array<VkBufferMemoryBarrier2, 0> buffer_memory_barrier{};

  CommandPipelineBarrier(std::views::single(image_memory_barrier), buffer_memory_barrier);
}

void CommandBuffer::CommandCopyBufferToImage(const Buffer &buffer, const Image &image, uint32_t mip_level, uint32_t base_layer,
                                             uint32_t layers, uint64_t buffer_offset, int32_t image_offset_x, int32_t image_offset_y,
                                             int32_t image_offset_z) {
  VkImageSubresourceLayers subresource_layers{};
  {
    subresource_layers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_layers.mipLevel = mip_level;
    subresource_layers.baseArrayLayer = base_layer;
    subresource_layers.layerCount = layers;
  }

  VkBufferImageCopy buffer_image_copy{};
  {
    buffer_image_copy.bufferOffset = buffer_offset;
    buffer_image_copy.bufferRowLength = 0;
    buffer_image_copy.bufferImageHeight = 0;
    buffer_image_copy.imageSubresource = subresource_layers;
    buffer_image_copy.imageOffset = VkOffset3D(image_offset_x, image_offset_y, image_offset_z);
    // buffer_image_copy.imageExtent = image.GetExtent();
  }

  VkImageLayout destination = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  // vkCmdCopyBufferToImage(command_buffer_, buffer, image, destination, 1, &buffer_image_copy);
}

} // namespace Innsmouth