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
  VK_CHECK(vkAllocateCommandBuffers(GraphicsContext::Get()->GetDevice(), &command_buffer_ai, &command_buffer_));
}

CommandBuffer::~CommandBuffer() {
}

void CommandBuffer::Submit() {
  SubmitInfo submit_info;
  {
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer_;
  }
  VK_CHECK(vkQueueSubmit(GraphicsContext::Get()->GetGeneralQueue(), 1, submit_info, VK_NULL_HANDLE));
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

void CommandBuffer::CommandBeginRendering(const Extent2D &extent, std::span<const RenderingAttachmentInfo> colors,
                                          const std::optional<RenderingAttachmentInfo> &depth,
                                          const std::optional<RenderingAttachmentInfo> &stencil) {
  RenderingInfo rendering_info;
  {
    rendering_info.renderArea.offset = {0, 0};
    rendering_info.renderArea.extent = extent;
    rendering_info.layerCount = 1;
    rendering_info.colorAttachmentCount = colors.size();
    rendering_info.pColorAttachments = colors.data();
    rendering_info.pDepthAttachment = depth.has_value() ? &depth.value() : nullptr;
    rendering_info.pStencilAttachment = stencil.has_value() ? &stencil.value() : nullptr;
  }

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
                                           std::span<const BufferMemoryBarrier2> buffer_barriers) {
  DependencyInfo dependency_info{};

  dependency_info.dependencyFlags = {};
  dependency_info.bufferMemoryBarrierCount = buffer_barriers.size();
  dependency_info.pBufferMemoryBarriers = buffer_barriers.data();
  dependency_info.imageMemoryBarrierCount = image_barriers.size();
  dependency_info.pImageMemoryBarriers = image_barriers.data();

  vkCmdPipelineBarrier2(command_buffer_, dependency_info);
}

void CommandBuffer::CommandImageMemoryBarrier(const VkImage &image, ImageLayout from_layout, ImageLayout to_layout,
                                              PipelineStageMask2 from_stage, PipelineStageMask2 to_stage, const ImageSubresourceRange &range) {

  auto from_access_mask = GetAccessMask(from_layout);
  auto to_access_mask = GetAccessMask(to_layout);

  std::array<ImageMemoryBarrier2, 1> image_memory_barrier = {};
  {
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

  std::array<BufferMemoryBarrier2, 0> buffer_memory_barrier{};

  CommandPipelineBarrier(image_memory_barrier, buffer_memory_barrier);
}

// BIND

void CommandBuffer::CommandBindGraphicsPipeline(const VkPipeline graphics_pipeline) {
  vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);
}

void CommandBuffer::CommandBindComputePipeline(const VkPipeline graphics_pipeline) {
  vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_COMPUTE, graphics_pipeline);
}

void CommandBuffer::CommandBindVertexBuffer(const VkBuffer buffer, std::size_t offset) {
  vkCmdBindVertexBuffers(command_buffer_, 0, 1, &buffer, &offset);
}

void CommandBuffer::CommandBindIndexBuffer(const VkBuffer buffer, std::size_t offset, VkIndexType index_type) {
  vkCmdBindIndexBuffer(command_buffer_, buffer, offset, index_type);
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

void CommandBuffer::CommandPushDescriptorSet(const VkPipelineLayout layout, uint32_t set_number, uint32_t binding, const VkImageView image_view,
                                             const VkSampler sampler) {
  VkDescriptorImageInfo descriptor_ii{};
  {
    descriptor_ii.sampler = sampler;
    descriptor_ii.imageView = image_view;
    descriptor_ii.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  }

  VkWriteDescriptorSet write_descriptor_set{};
  {
    write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_set.dstBinding = binding;
    write_descriptor_set.dstArrayElement = 0;
    write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write_descriptor_set.descriptorCount = 1;
    write_descriptor_set.pImageInfo = &descriptor_ii;
  }
  vkCmdPushDescriptorSetKHR(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, set_number, 1, &write_descriptor_set);
}

void CommandBuffer::CommandPushDescriptorSet(VkPipelineLayout layout, uint32_t set_number, uint32_t binding, VkBuffer buffer) {
  DescriptorBufferInfo descriptor_bi;
  {
    descriptor_bi.buffer = buffer;
    descriptor_bi.offset = 0;
    descriptor_bi.range = VK_WHOLE_SIZE;
  }

  WriteDescriptorSet write_descriptor_set;
  {
    write_descriptor_set.dstBinding = binding;
    write_descriptor_set.dstArrayElement = 0;
    write_descriptor_set.descriptorType = DescriptorType::E_STORAGE_BUFFER;
    write_descriptor_set.descriptorCount = 1;
    write_descriptor_set.pBufferInfo = &descriptor_bi;
  }
  vkCmdPushDescriptorSetKHR(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, set_number, 1, write_descriptor_set);
}

void CommandBuffer::CommandCopyBufferToImage(const VkBuffer buffer, const VkImage image, const VkExtent3D &extent) {
  VkImageSubresourceLayers subresource_layers{};
  {
    subresource_layers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_layers.mipLevel = 0;
    subresource_layers.baseArrayLayer = 0;
    subresource_layers.layerCount = 1;
  }

  VkBufferImageCopy buffer_image_copy{};
  {
    buffer_image_copy.bufferOffset = 0;
    buffer_image_copy.bufferRowLength = 0;
    buffer_image_copy.bufferImageHeight = 0;
    buffer_image_copy.imageSubresource = subresource_layers;
    buffer_image_copy.imageOffset = VkOffset3D(0, 0, 0);
    buffer_image_copy.imageExtent = extent;
  }

  vkCmdCopyBufferToImage(command_buffer_, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_image_copy);
}

} // namespace Innsmouth