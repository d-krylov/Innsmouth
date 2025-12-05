#include "buffer.h"
#include "innsmouth/core/include/core.h"

namespace Innsmouth {

Buffer::Buffer(std::size_t buffer_size, BufferUsageMask buffer_usage, AllocationCreateMask allocation_mask)
  : buffer_size_(buffer_size), buffer_usage_(buffer_usage) {
  auto buffer_information = CreateBuffer(buffer_size, buffer_usage, allocation_mask);
  buffer_ = buffer_information.buffer_;
  buffer_allocation_ = buffer_information.buffer_allocation_;
  mapped_memory_ = buffer_information.mapped_memory_;
}

Buffer::~Buffer() {
  GraphicsAllocator::Get()->DestroyBuffer(buffer_, buffer_allocation_);
}

Buffer::Buffer(Buffer &&other) noexcept {
  buffer_ = std::exchange(other.buffer_, VK_NULL_HANDLE);
  buffer_allocation_ = std::exchange(other.buffer_allocation_, VK_NULL_HANDLE);
  mapped_memory_ = std::exchange(other.mapped_memory_, nullptr);
  buffer_size_ = std::exchange(other.buffer_size_, 0);
  buffer_usage_ = std::exchange(other.buffer_usage_, BufferUsageMask());
}

Buffer &Buffer::operator=(Buffer &&other) noexcept {
  std::swap(buffer_, other.buffer_);
  std::swap(buffer_allocation_, other.buffer_allocation_);
  std::swap(mapped_memory_, other.mapped_memory_);
  std::swap(buffer_size_, other.buffer_size_);
  std::swap(buffer_usage_, other.buffer_usage_);
  return *this;
}

BufferInformation Buffer::CreateBuffer(std::size_t size, BufferUsageMask usage, AllocationCreateMask allocation_mask) {
  BufferCreateInfo buffer_ci;
  buffer_ci.size = size;
  buffer_ci.usage = usage;
  buffer_ci.sharingMode = SharingMode::E_EXCLUSIVE;

  BufferInformation buffer_information;
  auto allocation_information = GraphicsAllocator::Get()->AllocateBuffer(buffer_ci, buffer_information.buffer_, allocation_mask);

  buffer_information.buffer_allocation_ = allocation_information.allocation_;
  buffer_information.mapped_memory_ = allocation_information.mapped_memory_;
  return buffer_information;
}

VkBuffer Buffer::GetHandle() const {
  return buffer_;
}

void Buffer::Map() {
  GraphicsAllocator::Get()->MapMemory(buffer_allocation_, &mapped_memory_);
}

void Buffer::Unmap() {
  GraphicsAllocator::Get()->UnmapMemory(buffer_allocation_);
}

std::size_t Buffer::GetSize() const {
  return buffer_size_;
}

VkDeviceAddress Buffer::GetBufferAddress() const {
  BufferDeviceAddressInfo buffer_device_ai;
  buffer_device_ai.buffer = GetHandle();
  auto address = vkGetBufferDeviceAddress(GraphicsContext::Get()->GetDevice(), buffer_device_ai);
  return address;
}

} // namespace Innsmouth