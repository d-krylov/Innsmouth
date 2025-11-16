#include "buffer.h"

namespace Innsmouth {

Buffer::Buffer(std::size_t buffer_size, BufferUsageMask buffer_usage) : buffer_size_(buffer_size), buffer_usage_(buffer_usage) {
  CreateBuffer();
}

Buffer::~Buffer() {
}

void Buffer::CreateBuffer() {
  BufferCreateInfo buffer_ci;
  {
    buffer_ci.size = buffer_size_;
    buffer_ci.usage = buffer_usage_;
    buffer_ci.sharingMode = SharingMode::E_EXCLUSIVE;
  }

  vma_allocation_ = GraphicsAllocator::Get()->AllocateBuffer(buffer_ci, buffer_);
}

const VkBuffer Buffer::GetHandle() const {
  return buffer_;
}

void Buffer::Map() {
  mapped_data_ = GraphicsAllocator::Get()->MapMemory(vma_allocation_);
}

void Buffer::Unmap() {
  GraphicsAllocator::Get()->UnmapMemory(vma_allocation_);
}

VkDeviceAddress Buffer::GetBufferAddress() const {
  BufferDeviceAddressInfo buffer_device_ai;
  buffer_device_ai.buffer = GetHandle();
  auto address = vkGetBufferDeviceAddress(GraphicsContext::Get()->GetDevice(), buffer_device_ai);
  return address;
}

} // namespace Innsmouth