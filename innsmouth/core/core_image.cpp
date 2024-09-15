#include "innsmouth/core/include/core_image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace Innsmouth {

CoreImage::CoreImage(const std::filesystem::path &path) {
  int32_t channels;

  stbi_set_flip_vertically_on_load(true);

  std::byte *pixels =
    reinterpret_cast<std::byte *>(stbi_load(path.c_str(), &size_.x, &size_.y, &channels, 0));

  if (pixels != nullptr) {
    if (channels == 3) {
      data_.resize(size_.x * size_.y * IMAGE_CHANNELS);
      for (int i = 0; i < size_.x * size_.y; ++i) {
        data_[i * 4 + 0] = pixels[i * 3];
        data_[i * 4 + 1] = pixels[i * 3 + 1];
        data_[i * 4 + 2] = pixels[i * 3 + 2];
        data_[i * 4 + 3] = std::byte(0xff);
      }
    } else {
      data_ = std::vector<std::byte>(pixels, pixels + GetLength());
    }
  }
}

} // namespace Innsmouth