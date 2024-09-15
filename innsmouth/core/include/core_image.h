#ifndef INNSMOUTH_CORE_IMAGE_H
#define INNSMOUTH_CORE_IMAGE_H

#include "core_types.h"
#include <filesystem>
#include <memory>
#include <vector>

namespace Innsmouth {

class CoreImage {

public:
  CoreImage(const std::filesystem::path &path);

  CoreImage();

  ~CoreImage() = default;

  [[nodiscard]] uint32_t GetWidth() const { return uint32_t(size_.x); }
  [[nodiscard]] uint32_t GetHeight() const { return uint32_t(size_.y); }
  [[nodiscard]] const Vector2i &GetSize() const { return size_; }
  [[nodiscard]] std::size_t GetLength() const { return size_.x * size_.y * IMAGE_CHANNELS; }

  [[nodiscard]] std::span<const std::byte> GetData() const { return std::span<const std::byte>(data_); }

private:
  std::vector<std::byte> data_;
  Vector2i size_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_CORE_IMAGE_H