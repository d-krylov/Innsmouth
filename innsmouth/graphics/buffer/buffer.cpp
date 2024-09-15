#include "innsmouth/graphics/buffer/buffer.h"
#include "innsmouth/core/include/core_types.h"
#include "innsmouth/graphics/include/graphics.h"

namespace Innsmouth {

Buffer::Buffer(BufferUsage buffer_usage, VkDeviceSize size, VmaMemoryUsage memory_usage)
  : buffer_usage_(buffer_usage), size_(size) {

  VkBufferCreateInfo buffer_ci{};
  {
    buffer_ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_ci.size = size_;
    buffer_ci.usage = VkBufferUsageFlags(buffer_usage_);
    buffer_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  VmaAllocationCreateInfo vma_allocation_ci{};
  {
    vma_allocation_ci.usage = memory_usage;
    vma_allocation_ci.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
  }

  VK_CHECK(vmaCreateBuffer(Allocator(), &buffer_ci, &vma_allocation_ci, &buffer_, &allocation_, nullptr));
}

Buffer::~Buffer() { vmaDestroyBuffer(Allocator(), buffer_, allocation_); }

std::span<std::byte> Buffer::Map(VkDeviceSize size, VkDeviceSize offset) {
  std::byte *mapped{nullptr};
  VK_CHECK(vmaMapMemory(Allocator(), allocation_, reinterpret_cast<void **>(&mapped)));
  mapped_ = std::span(mapped, size);
  return mapped_;
}

void Buffer::Unmap() {
  if (mapped_.empty() == false) {
    vmaUnmapMemory(Allocator(), allocation_);
    mapped_ = std::span<std::byte>();
  }
}

void Buffer::Flush(VkDeviceSize size, VkDeviceSize offset) {
  vmaFlushAllocation(Allocator(), allocation_, offset, size);
}

} // namespace Innsmouth