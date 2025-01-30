#ifndef INNSMOUTH_BUFFER_H
#define INNSMOUTH_BUFFER_H

#include "graphics/include/graphics_types.h"
#include <span>

namespace Innsmouth {

class Buffer {
public:
  Buffer(std::size_t size, BufferUsage usage, MemoryProperty memory_property, MemoryUsage memory_usage);

  ~Buffer();

  void Flush(uint64_t size, uint64_t offset);

private:
  VkBuffer buffer_{VK_NULL_HANDLE};
  VmaAllocation vma_allocation_{VK_NULL_HANDLE};
  std::span<std::byte> mapped_data_;
  std::size_t size_;
  BufferUsage usage_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_BUFFER_H