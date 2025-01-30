#include "buffer.h"
#include "graphics/include/graphics.h"

namespace Innsmouth {

Buffer::Buffer(std::size_t size, BufferUsage usage, MemoryProperty memory_property, MemoryUsage memory_usage) : size_(size), usage_(usage) {

  auto is_mappable = HasBits(memory_property, MemoryProperty::HOST_VISIBLE_BIT);

  VkBufferCreateInfo buffer_ci{};
  {
    buffer_ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_ci.size = size_;
    buffer_ci.usage = std::to_underlying(usage_);
    buffer_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  VmaAllocationCreateInfo vma_allocation_ci{};
  {
    vma_allocation_ci.usage = VmaMemoryUsage(memory_usage);
    vma_allocation_ci.flags = is_mappable ? VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT : 0;
    vma_allocation_ci.preferredFlags = std::to_underlying(memory_property);
  }

  VK_CHECK(vmaCreateBuffer(Allocator(), &buffer_ci, &vma_allocation_ci, &buffer_, &vma_allocation_, nullptr));
}

Buffer::~Buffer() {}

void Buffer::Flush(uint64_t size, uint64_t offset) { vmaFlushAllocation(Allocator(), vma_allocation_, offset, size); }

} // namespace Innsmouth