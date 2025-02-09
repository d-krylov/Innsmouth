#include "image.h"
#include "graphics/include/graphics.h"
#include "graphics/buffer/buffer.h"
#include "graphics/buffer/command_buffer.h"
#include "graphics/include/structure_tools.h"

namespace Innsmouth {

Image::Image(uint32_t width, uint32_t height, uint32_t depth) : extent_(width, height, depth) {}

VkImage Image::CreateImage(VmaAllocation &allocation, const VkImageCreateInfo &image_ci) {
  VmaAllocationCreateInfo allocation_ci{};
  {
    allocation_ci.flags = 0;
    allocation_ci.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    allocation_ci.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    allocation_ci.preferredFlags = 0;
    allocation_ci.memoryTypeBits = 0;
    allocation_ci.pool = nullptr;
    allocation_ci.pUserData = nullptr;
  }

  VmaAllocationInfo allocation_info{};
  VkImage image{};

  VK_CHECK(vmaCreateImage(Allocator(), &image_ci, &allocation_ci, &image, &allocation, &allocation_info));

  return image;
}

VkImageView Image::CreateImageView(const VkImage image, Format format, ImageViewType type, const VkImageSubresourceRange &range) {
  VkImageView image_view{};
  VkImageViewCreateInfo image_view_ci{};
  {
    image_view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_ci.image = image;
    image_view_ci.viewType = VkImageViewType(type);
    image_view_ci.format = VkFormat(format);
    image_view_ci.subresourceRange = range;
  }

  VK_CHECK(vkCreateImageView(Device(), &image_view_ci, nullptr, &image_view));

  return image_view;
}

void Image::SetData(std::span<const std::byte> data) {
  Buffer buffer(data.size(), BufferUsage::TRANSFER_SRC_BIT);
  buffer.Map();
  buffer.Memcpy<std::byte>(data);
  buffer.Unmap();

  CommandBuffer command_buffer(GraphicsCommandPool());
  command_buffer.Begin();
  auto range = CreateImageSubresourceRange();
  command_buffer.CommandImageMemoryBarrier(image_, ImageLayout::UNDEFINED, ImageLayout::TRANSFER_DST_OPTIMAL,
                                           PipelineStage::TOP_OF_PIPE_BIT, PipelineStage::TRANSFER_BIT, range);

  command_buffer.CommandCopyBufferToImage(buffer, *this, 0);

  command_buffer.CommandImageMemoryBarrier(image_, ImageLayout::TRANSFER_DST_OPTIMAL, ImageLayout::SHADER_READ_ONLY_OPTIMAL,
                                           PipelineStage::TRANSFER_BIT, PipelineStage::ALL_COMMANDS_BIT, range);

  command_buffer.End();

  command_buffer.Submit();
}

} // namespace Innsmouth