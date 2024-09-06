#ifndef INNSMOUTH_BUFFER_H
#define INNSMOUTH_BUFFER_H

#include "innsmouth/graphics/descriptors/write_descriptor_set.h"
#include "innsmouth/graphics/include/graphics_types.h"
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

  [[nodiscard]] VkDeviceSize GetSize() const { return size_; }
  [[nodiscard]] VkBufferUsageFlags GetUsage() const { return buffer_usage_; }

  template <typename T> void SetData(std::span<T> data) { SetData(std::as_bytes(data)); }

  void SetData(std::span<const std::byte> source);
  void Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

  std::span<std::byte> Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  void Unmap();

  [[nodiscard]] WriteDescriptorSet GetWriteDescriptorSet(uint32_t binding, VkDescriptorType type,
                                                         VkDeviceSize offset = 0,
                                                         VkDeviceSize size = VK_WHOLE_SIZE) const;

  std::span<std::byte> GetMapped() const { return mapped_; }

private:
  VkBuffer buffer_{VK_NULL_HANDLE};
  VkBufferUsageFlags buffer_usage_;
  VkDeviceSize size_{0};
  VmaAllocation allocation_{VK_NULL_HANDLE};
  std::span<std::byte> mapped_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_BUFFER_H