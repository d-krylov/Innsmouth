#include "stb_image.h"
#include "core/include/core_image.h"

namespace Innsmouth {

CoreImage::CoreImage(const std::filesystem::path &path) {
  stbi_set_flip_vertically_on_load(true);
  auto *pixels = reinterpret_cast<std::byte *>(stbi_load(path.c_str(), &width_, &height_, nullptr, STBI_rgb_alpha));
  data_ = std::vector<std::byte>(pixels, pixels + GetLength());
  stbi_image_free(pixels);
}

} // namespace Innsmouth
