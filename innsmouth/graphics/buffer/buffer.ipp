#ifndef INNSMOUTH_BUFFER_IPP
#define INNSMOUTH_BUFFER_IPP

namespace Innsmouth {

template <typename T> std::span<T> Buffer::GetMappedData() {
  auto out_data = reinterpret_cast<T *>(mapped_memory_);
  auto out_size = buffer_size_ / sizeof(T);
  return std::span(out_data, out_size);
}

template <typename T> void Buffer::SetData(std::span<const T> data, std::size_t byte_offset) {
  GraphicsAllocator::Get()->CopyMemoryToAllocation(std::as_bytes(data), vma_allocation_, byte_offset);
}

} // namespace Innsmouth

#endif // INNSMOUTH_BUFFER_IPP