#include "innsmouth/graphics/buffer/buffer.h"
#include "innsmouth/core/include/core_types.h"
#include "innsmouth/graphics/include/graphics.h"
#include <cstring>

namespace Innsmouth {

Buffer::Buffer(BufferUsage buffer_usage, VkDeviceSize size, VmaMemoryUsage memory_usage)
  : buffer_usage_(VkBufferUsageFlags(buffer_usage)), size_(size) {

  VkBufferCreateInfo buffer_ci{};
  {
    buffer_ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_ci.size = size_;
    buffer_ci.usage = buffer_usage_;
    buffer_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  VmaAllocationCreateInfo vma_allocation_ci{};
  {
    vma_allocation_ci.usage = memory_usage;
    vma_allocation_ci.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
  }

  vmaCreateBuffer(Allocator(), &buffer_ci, &vma_allocation_ci, &buffer_, &allocation_, nullptr);
}

Buffer::~Buffer() { vmaDestroyBuffer(Allocator(), buffer_, allocation_); }

std::span<std::byte> Buffer::Map(VkDeviceSize size, VkDeviceSize offset) {
  void *mapped{nullptr};
  VK_CHECK(vmaMapMemory(Allocator(), allocation_, &mapped));
  mapped_ = std::span<std::byte>(ToBytePointer(mapped), size);
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

void Buffer::SetData(std::span<const std::byte> source) {
  Map();
  std::memcpy(mapped_.data(), source.data(), source.size());
  Unmap();
}

WriteDescriptorSet Buffer::GetWriteDescriptorSet(uint32_t binding, VkDescriptorType type,
                                                 VkDeviceSize offset, VkDeviceSize size) const {
  VkDescriptorBufferInfo descriptor_bi{};
  {
    descriptor_bi.buffer = buffer_;
    descriptor_bi.offset = offset;
    descriptor_bi.range = size;
  }

  VkWriteDescriptorSet write_descriptor_set{};
  {
    write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_set.dstSet = VK_NULL_HANDLE;
    write_descriptor_set.dstBinding = binding;
    write_descriptor_set.dstArrayElement = 0;
    write_descriptor_set.descriptorType = type;
    write_descriptor_set.descriptorCount = 1;
  }

  return WriteDescriptorSet(descriptor_bi, write_descriptor_set);
}

} // namespace Innsmouth