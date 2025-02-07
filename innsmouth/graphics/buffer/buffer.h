#ifndef INNSMOUTH_BUFFER_H
#define INNSMOUTH_BUFFER_H

#include "graphics/include/graphics_types.h"
#include <cstring>
#include <span>
#include <bit>

namespace Innsmouth {

class Buffer {
public:
  Buffer(std::size_t size, BufferUsage usage, MemoryProperty memory_property = MemoryProperty::HOST_VISIBLE_BIT,
         MemoryUsage memory_usage = MemoryUsage::AUTO);

  ~Buffer();

  void Flush();

  void Map();

  void Unmap();

  auto begin() { return mapped_data_.begin(); }

  auto begin() const { return mapped_data_.begin(); }

  auto end() { return mapped_data_.end(); }

  auto end() const { return mapped_data_.end(); }

  operator const VkBuffer &() const { return buffer_; }

  const VkBuffer *get() const { return std::addressof(buffer_); }

  const VkBuffer GetBuffer() const { return buffer_; }

  std::span<std::byte> GetMappedData() { return mapped_data_; }

  template <typename T> void Memcpy(std::span<const T> data, std::size_t offset = 0);

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