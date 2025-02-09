#ifndef INNSMOUTH_CORE_IMAGE_H
#define INNSMOUTH_CORE_IMAGE_H

#include "core_types.h"
#include <vector>

namespace Innsmouth {

class CoreImage {
public:
  CoreImage(const std::filesystem::path &path);

  int32_t GetWidth() const { return width_; }
  int32_t GetHeight() const { return height_; }

  std::size_t GetLength() const { return width_ * height_ * channels_; }

  std::span<const std::byte> GetData() const { return data_; }

private:
  std::vector<std::byte> data_;
  int32_t width_;
  int32_t height_;
  int32_t channels_{4};
};

} // namespace Innsmouth

#endif // INNSMOUTH_CORE_IMAGE_H
