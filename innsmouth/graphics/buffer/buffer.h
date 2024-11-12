#ifndef INNSMOUTH_BUFFER_H
#define INNSMOUTH_BUFFER_H

#include "core/include/concepts.h"
#include "graphics/include/graphics_types.h"
#include <cstring>
#include <span>

namespace Innsmouth {

class Buffer {
public:
  Buffer(BufferUsage buffer_usage, VkDeviceSize size,
         VmaMemoryUsage memory_usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO);

  ~Buffer();

  NO_COPY_SEMANTIC(Buffer);

  operator const VkBuffer &() const { return buffer_; }
  [[nodiscard]] const VkBuffer *get() const { return &buffer_; }

  [[nodiscard]] uint64_t GetSize() const { return size_; }
  [[nodiscard]] BufferUsage GetUsage() const { return buffer_usage_; }

  template <typename R>
    requires std::ranges::contiguous_range<R> && std::ranges::sized_range<R>
  void SetData(R &&source) {
    Map();
    auto size = std::ranges::size(source) * sizeof(std::ranges::range_value_t<R>);
    std::memcpy(mapped_.data(), std::ranges::data(source), size);
    Flush();
    Unmap();
  }

  void Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

  std::span<std::byte> Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  void Unmap();

  std::span<std::byte> GetMapped() const { return mapped_; }

private:
  VkBuffer buffer_{VK_NULL_HANDLE};
  BufferUsage buffer_usage_;
  uint64_t size_{0};
  VmaAllocation allocation_{VK_NULL_HANDLE};
  std::span<std::byte> mapped_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_BUFFER_H