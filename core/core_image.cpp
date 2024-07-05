#include "core/include/core_image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace Innsmouth {

CoreImage::CoreImage(const std::filesystem::path &path) {
  auto data = ToBytePointer(stbi_load(path.c_str(), &size_.x, &size_.y, &channels_, 0));
  if (data != nullptr) {
    data_ = std::span<std::byte>(data, size_.x * size_.y * channels_);
  }
}

} // namespace Innsmouth