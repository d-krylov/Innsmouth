#ifndef INNSMOUTH_BUFFER_H
#define INNSMOUTH_BUFFER_H

#include "graphics/include/graphics_types.h"
#include <span>
#include <bit>

namespace Innsmouth {

class Buffer {
public:
  Buffer(std::size_t size, BufferUsage usage, MemoryProperty memory_property, MemoryUsage memory_usage);

  ~Buffer();

  void Flush(uint64_t size, uint64_t offset);

  void Map();
  void Unmap();

  const VkBuffer GetBuffer() const { return buffer_; }

  std::span<std::byte> GetMappedData() { return mapped_data_; }

private:
  VkBuffer buffer_{VK_NULL_HANDLE};
  VmaAllocation vma_allocation_{VK_NULL_HANDLE};
  std::span<std::byte> mapped_data_;
  std::size_t size_;
  BufferUsage usage_;
};

} // namespace Innsmouth

#include "buffer.ipp"

#endif // INNSMOUTH_BUFFER_H