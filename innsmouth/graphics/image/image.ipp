#ifndef INNSMOUTH_IMAGE_IPP
#define INNSMOUTH_IMAGE_IPP

namespace Innsmouth {

template <typename T> void Image::SetData(std::span<const T> data) {
  SetImageData(std::as_bytes(data));
}

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_IPP