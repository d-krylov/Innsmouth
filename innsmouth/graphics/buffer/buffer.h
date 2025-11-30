#ifndef INNSMOUTH_BUFFER_H
#define INNSMOUTH_BUFFER_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"
#include "innsmouth/graphics/graphics_context/graphics_allocator.h"
#include <span>

namespace Innsmouth {

class Buffer {
public:
  static constexpr AllocationCreateMask CPU = AllocationCreateMaskBits::E_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
  static constexpr AllocationCreateMask MAPPED = CPU | AllocationCreateMaskBits::E_MAPPED_BIT;

  Buffer() = default;

  Buffer(std::size_t buffer_size, BufferUsageMask buffer_usage, AllocationCreateMask allocation_mask);

  ~Buffer();

  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;

  Buffer(Buffer &&other) noexcept;
  Buffer &operator=(Buffer &&other) noexcept;

  void Map();
  void Unmap();

  template <typename T> std::span<T> GetMappedData();
  template <typename T> void SetData(std::span<const T> data, std::size_t byte_offset = 0);

  VkBuffer GetHandle() const;
  VkDeviceAddress GetBufferAddress() const;

  std::size_t GetSize() const;

protected:
  void CreateBuffer(AllocationCreateMask allocation_mask);

private:
  VkBuffer buffer_{VK_NULL_HANDLE};
  VmaAllocation vma_allocation_{VK_NULL_HANDLE};
  std::size_t buffer_size_{0};
  std::byte *mapped_memory_{nullptr};
  BufferUsageMask buffer_usage_;
};

} // namespace Innsmouth

#include "buffer.ipp"

#endif // INNSMOUTH_BUFFER_H