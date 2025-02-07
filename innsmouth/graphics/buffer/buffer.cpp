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

void Buffer::Map() {
  std::byte *mapped_data{nullptr};

  if (mapped_data_.empty()) {

    VK_CHECK(vmaMapMemory(Allocator(), vma_allocation_, reinterpret_cast<void **>(&mapped_data)));

    mapped_data_ = std::span(mapped_data, size_);
  }
}

void Buffer::Unmap() {
  if (mapped_data_.empty() == false) {

    vmaUnmapMemory(Allocator(), vma_allocation_);

    mapped_data_ = std::span<std::byte>();
  }
}

void Buffer::Flush() { vmaFlushAllocation(Allocator(), vma_allocation_, 0, size_); }

} // namespace Innsmouth