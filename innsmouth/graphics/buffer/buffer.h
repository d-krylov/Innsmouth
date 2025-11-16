#ifndef INNSMOUTH_BUFFER_H
#define INNSMOUTH_BUFFER_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"
#include "innsmouth/graphics/graphics_context/graphics_allocator.h"
#include <span>

namespace Innsmouth {

class Buffer {
public:
  Buffer(std::size_t buffer_size, BufferUsageMask buffer_usage);

  ~Buffer();

  void Map();

  void Unmap();

  template <typename T> std::span<T> GetMappedData();

  const VkBuffer GetHandle() const;

  VkDeviceAddress GetBufferAddress() const;

protected:
  void CreateBuffer();

private:
  VkBuffer buffer_{VK_NULL_HANDLE};
  VmaAllocation vma_allocation_{VK_NULL_HANDLE};
  std::size_t buffer_size_{0};
  std::byte *mapped_data_{nullptr};
  BufferUsageMask buffer_usage_;
};

} // namespace Innsmouth

#include "buffer.ipp"

#endif // INNSMOUTH_BUFFER_H