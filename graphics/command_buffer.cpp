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

CommandBuffer::~CommandBuffer() {}

CommandBuffer::CommandBuffer(CommandBuffer &&other) noexcept {
  command_buffer_ = std::exchange(other.command_buffer_, VK_NULL_HANDLE);
}

void CommandBuffer::Begin(CommandBufferUsage usage) {
  VkCommandBufferBeginInfo command_buffer_bi{};
  {
    command_buffer_bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_bi.flags = static_cast<VkCommandBufferUsageFlags>(usage);
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

void CommandBuffer::CommandSetCullMode(bool front, bool back) {
  VkCullModeFlags f = front ? VK_CULL_MODE_FRONT_BIT : VK_CULL_MODE_NONE;
  if (back) {
    f |= VK_CULL_MODE_BACK_BIT;
  }
  vkCmdSetCullMode(command_buffer_, f);
}

void CommandBuffer::CommandSetFrontFace(FrontFace front_face) {
  vkCmdSetFrontFace(command_buffer_, VkFrontFace(front_face));
}

void CommandBuffer::CommandEnableDepthTest(bool b) {
  vkCmdSetDepthTestEnable(command_buffer_, b ? VK_TRUE : VK_FALSE);
}

void CommandBuffer::CommandEnableStencilTest(bool b) {
  vkCmdSetStencilTestEnable(command_buffer_, b ? VK_TRUE : VK_FALSE);
}

void CommandBuffer::CommandBeginRendering(const VkImageView image_view, const VkExtent2D &extent) {
  VkRenderingAttachmentInfo rendering_ai{};
  {
    rendering_ai.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    rendering_ai.imageView = image_view;
    rendering_ai.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    rendering_ai.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    rendering_ai.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    rendering_ai.clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};
  }

  VkRenderingAttachmentInfo depthAttachment = {};

  VkRenderingInfo rendering_info{};
  {
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    rendering_info.renderArea = {0, 0, extent.width, extent.height};
    rendering_info.layerCount = 1;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachments = &rendering_ai;
    rendering_info.pDepthAttachment = nullptr;
    rendering_info.pStencilAttachment = nullptr;
  }
  vkCmdBeginRendering(command_buffer_, &rendering_info);
}

void CommandBuffer::CommandEndRendering() { vkCmdEndRenderingKHR(command_buffer_); }

void CommandBuffer::CommandSetPrimitiveTopology(PrimitiveTopology topology) {
  vkCmdSetPrimitiveTopology(command_buffer_, VkPrimitiveTopology(topology));
}

void CommandBuffer::CommandSetColorBlendEquation() {
  VkColorBlendEquationEXT color_blend_equation{};
  {
    color_blend_equation.alphaBlendOp = VK_BLEND_OP_ADD;
    color_blend_equation.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_equation.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_equation.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_equation.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_equation.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  }
  vkCmdSetColorBlendEquationEXT(command_buffer_, 0, 1, &color_blend_equation);
}

void CommandBuffer::CommandSetScissor(const VkRect2D &scissor) {
  vkCmdSetScissor(command_buffer_, 0, 1, &scissor);
}

void CommandBuffer::CommandEnableBlend(bool b) {
  VkBool32 enable = b ? VK_TRUE : VK_FALSE;
  vkCmdSetColorBlendEnableEXT(command_buffer_, 0, 1, &enable);
}

void CommandBuffer::CommandBindPipeline(const GraphicsPipeline &graphics_pipeline) {
  vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);
}

void CommandBuffer::CommandBindVertexBuffer(const Buffer &buffer, VkDeviceSize offset) {
  vkCmdBindVertexBuffers(command_buffer_, 0, 1, buffer.get(), &offset);
}

void CommandBuffer::CommandBindIndexBuffer(const Buffer &buffer, VkDeviceSize offset,
                                           VkIndexType index_type) {
  vkCmdBindIndexBuffer(command_buffer_, buffer, offset, index_type);
}

void CommandBuffer::CommandDraw(uint32_t vertex_count, uint32_t instance_count,
                                uint32_t first_vertex, uint32_t first_instance) {
  vkCmdDraw(command_buffer_, vertex_count, instance_count, first_vertex, first_instance);
}

void CommandBuffer::CommandDrawIndexed(uint32_t index_count, uint32_t instance_count,
                                       uint32_t first_index, int32_t vertex_offset,
                                       uint32_t first_instance) {
  vkCmdDrawIndexed(command_buffer_, index_count, instance_count, first_index, vertex_offset,
                   first_instance);
}

void CommandBuffer::CommandPushConstants(const GraphicsPipeline &graphics_pipeline,
                                         ShaderStage stage, std::span<const std::byte> d,
                                         uint32_t offset) {
  vkCmdPushConstants(command_buffer_, graphics_pipeline.GetPipelineLayout(),
                     VkShaderStageFlags(stage), offset, d.size(), d.data());
}

void CommandBuffer::CommandPushDescriptorSet(const GraphicsPipeline &graphics_pipeline,
                                             uint32_t set_number,
                                             std::span<const VkWriteDescriptorSet> sets) {
  vkCmdPushDescriptorSetKHR(command_buffer_, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
                            graphics_pipeline.GetPipelineLayout(), set_number, sets.size(),
                            sets.data());
}

void CommandBuffer::CommandCopyBufferToImage(const Buffer &buffer, const Image &image,
                                             const VkExtent3D &extent, uint32_t level,
                                             uint32_t base_layer, uint32_t layers,
                                             VkDeviceSize buffer_offset,
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
                                          const VkImageSubresourceRange &range,
                                          PipelineStage source, PipelineStage destination) {
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

  vkCmdPipelineBarrier(command_buffer_, VkPipelineStageFlags(source),
                       VkPipelineStageFlags(destination), 0, 0, nullptr, 0, nullptr, 1,
                       &image_memory_barrier);
}

} // namespace Innsmouth