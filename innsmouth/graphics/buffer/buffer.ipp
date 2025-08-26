#ifndef INNSMOUTH_BUFFER_IPP
#define INNSMOUTH_BUFFER_IPP

namespace Innsmouth {

template <typename T> std::span<T> Buffer::GetMappedData() {
  auto out_data = reinterpret_cast<T *>(mapped_data_);
  auto out_size = buffer_size_ / sizeof(T);
  return std::span(out_data, out_size);
}

} // namespace Innsmouth

#endif // INNSMOUTH_BUFFER_IPP