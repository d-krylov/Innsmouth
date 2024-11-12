#include "graphics/include/graphics_common.h"

namespace Innsmouth {

CommandBuffer::CommandBuffer(const VkCommandPool command_pool, bool begin) {
  VkCommandBufferAllocateInfo command_buffer_ai{};
  {
    command_buffer_ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_ai.commandPool = command_pool;
    command_buffer_ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_ai.commandBufferCount = 1;
  }
  VK_CHECK(vkAllocateCommandBuffers(Device(), &command_buffer_ai, &command_buffer_));

  if (begin) {
    Begin();
  }
}

CommandBuffer::CommandBuffer(CommandBuffer &&other) noexcept {
  command_buffer_ = std::exchange(other.command_buffer_, VK_NULL_HANDLE);
}

void CommandBuffer::Begin(CommandBufferUsage usage) {
  VkCommandBufferBeginInfo command_buffer_bi{};
  {
    command_buffer_bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_bi.flags = VkCommandBufferUsageFlags(usage);
  }
  VK_CHECK(vkBeginCommandBuffer(command_buffer_, &command_buffer_bi));
}

void CommandBuffer::End() { VK_CHECK(vkEndCommandBuffer(command_buffer_)); }

void CommandBuffer::Reset() { vkResetCommandBuffer(command_buffer_, 0); }

void CommandBuffer::Flush() {
  End();

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

void CommandBuffer::CommandSetViewport(float x, float y, float w, float h) {
  VkViewport viewport{};
  {
    viewport.x = x;
    viewport.y = y;
    viewport.width = w;
    viewport.height = h;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
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

void CommandBuffer::CommandSetCullMode(CullMode mode) {
  vkCmdSetCullMode(command_buffer_, VkCullModeFlags(mode));
}

void CommandBuffer::CommandSetFrontFace(FrontFace front_face) {
  vkCmdSetFrontFace(command_buffer_, VkFrontFace(front_face));
}

void CommandBuffer::CommandEnableDepthTest(bool b) { vkCmdSetDepthTestEnable(command_buffer_, uint32_t(b)); }

void CommandBuffer::CommandEnableDepthWrite(bool b) {
  vkCmdSetDepthWriteEnable(command_buffer_, uint32_t(b));
}

void CommandBuffer::CommandEnableStencilTest(bool b) {
  vkCmdSetStencilTestEnable(command_buffer_, uint32_t(b));
}

void CommandBuffer::CommandDepthCompareOperation(CompareOperation compare_operation) {
  vkCmdSetDepthCompareOp(command_buffer_, VkCompareOp(compare_operation));
}

void CommandBuffer::CommandDepthBounds(float min, float max) {
  vkCmdSetDepthBounds(command_buffer_, min, max);
}

void CommandBuffer::CommandEndRendering() { vkCmdEndRenderingKHR(command_buffer_); }

void CommandBuffer::CommandSetPrimitiveTopology(PrimitiveTopology topology) {
  vkCmdSetPrimitiveTopology(command_buffer_, VkPrimitiveTopology(topology));
}

void CommandBuffer::CommandBindPipeline(const GraphicsPipeline &graphics_pipeline) {
  vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);
}

void CommandBuffer::CommandBindVertexBuffer(const Buffer &buffer, VkDeviceSize offset) {
  vkCmdBindVertexBuffers(command_buffer_, 0, 1, buffer.get(), &offset);
}

void CommandBuffer::CommandPushDescriptorSet(const GraphicsPipeline &graphics_pipeline, uint32_t set_number,
                                             uint32_t binding, const Image &image) {

  VkDescriptorImageInfo descriptor_ii{};
  {
    descriptor_ii.sampler = image.GetSampler();
    descriptor_ii.imageView = image.GetImageView();
    descriptor_ii.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  }

  VkWriteDescriptorSet write_descriptor_set{};
  {
    write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_set.dstBinding = binding;
    write_descriptor_set.dstArrayElement = 0;
    write_descriptor_set.descriptorType = VkDescriptorType(DescriptorType::COMBINED_IMAGE_SAMPLER);
    write_descriptor_set.descriptorCount = 1;
    write_descriptor_set.pImageInfo = &descriptor_ii;
  }

  vkCmdPushDescriptorSetKHR(command_buffer_, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
                            graphics_pipeline.GetPipelineLayout(), set_number, 1, &write_descriptor_set);
}

void CommandBuffer::CommandPushDescriptorSet(const GraphicsPipeline &graphics_pipeline, uint32_t set_number,
                                             uint32_t binding, const Buffer &buffer, uint64_t offset,
                                             uint64_t size) {

  VkDescriptorBufferInfo descriptor_bi{};
  {
    descriptor_bi.buffer = buffer;
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

  vkCmdPushDescriptorSetKHR(command_buffer_, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
                            graphics_pipeline.GetPipelineLayout(), set_number, 1, &write_descriptor_set);
}

void CommandBuffer::CommandBindIndexBuffer(const Buffer &buffer, VkDeviceSize offset,
                                           VkIndexType index_type) {
  vkCmdBindIndexBuffer(command_buffer_, buffer, offset, index_type);
}

void CommandBuffer::CommandDraw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
                                uint32_t first_instance) {
  vkCmdDraw(command_buffer_, vertex_count, instance_count, first_vertex, first_instance);
}

void CommandBuffer::CommandDrawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index,
                                       int32_t vertex_offset, uint32_t first_instance) {
  vkCmdDrawIndexed(command_buffer_, index_count, instance_count, first_index, vertex_offset, first_instance);
}

void CommandBuffer::CommandCopyBufferToImage(const Buffer &buffer, const Image &image,
                                             const VkExtent3D &extent, uint32_t level, uint32_t base_layer,
                                             uint32_t layers, VkDeviceSize buffer_offset,
                                             const VkOffset3D &image_offset) {
  VkImageSubresourceLayers subresource_layers{};
  {
    subresource_layers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_layers.mipLevel = level;
    subresource_layers.baseArrayLayer = base_layer;
    subresource_layers.layerCount = layers;
  }

  VkBufferImageCopy buffer_image_copy{};
  {
    buffer_image_copy.bufferOffset = buffer_offset;
    buffer_image_copy.bufferRowLength = 0;
    buffer_image_copy.bufferImageHeight = 0;
    buffer_image_copy.imageSubresource = subresource_layers;
    buffer_image_copy.imageOffset = image_offset;
    buffer_image_copy.imageExtent = extent;
  }

  VkImageLayout destination = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  vkCmdCopyBufferToImage(command_buffer_, buffer, image, destination, 1, &buffer_image_copy);
}

void CommandBuffer::CommandSetImageLayout(const VkImage &image, ImageLayout from, ImageLayout to,
                                          const VkImageSubresourceRange &range, PipelineStage source,
                                          PipelineStage destination) {
  auto access_mask = GetAccessMask(VkImageLayout(from));

  VkImageMemoryBarrier image_memory_barrier{};
  {
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.oldLayout = VkImageLayout(from);
    image_memory_barrier.newLayout = VkImageLayout(to);
    image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.image = image;
    image_memory_barrier.subresourceRange = range;
    image_memory_barrier.srcAccessMask = access_mask;
    image_memory_barrier.dstAccessMask = access_mask;
  }

  if (to == ImageLayout::SHADER_READ_ONLY_OPTIMAL) {
    if (access_mask == 0) {
      image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
    }
  }

  vkCmdPipelineBarrier(command_buffer_, VkPipelineStageFlags(source), VkPipelineStageFlags(destination), 0, 0,
                       nullptr, 0, nullptr, 1, &image_memory_barrier);
}

} // namespace Innsmouth