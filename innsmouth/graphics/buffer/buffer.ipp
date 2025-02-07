#ifndef INNSMOUTH_BUFFER_IPP
#define INNSMOUTH_BUFFER_IPP

namespace Innsmouth {

template <typename T> void Buffer::Memcpy(std::span<const T> source_data, std::size_t offset_in_elements) {
  auto offset_in_bytes = sizeof(T) * offset_in_elements;
  std::memcpy(mapped_data_.subspan(offset_in_bytes).data(), source_data.data(), source_data.size_bytes());
}

} // namespace Innsmouth

#endif // INNSMOUTH_BUFFER_IPP