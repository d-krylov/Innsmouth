#ifndef INNSMOUTH_CORE_IMAGE_H
#define INNSMOUTH_CORE_IMAGE_H

#include "core_types.h"
#include <filesystem>
#include <memory>

namespace Innsmouth {

class CoreImage {

public:
  CoreImage(const std::filesystem::path &path);

  ~CoreImage() = default;

  [[nodiscard]] const Vector2i &GetSize() const { return size_; }
  [[nodiscard]] std::size_t GetLength() const { return size_.x * size_.y * channels_; }

  [[nodiscard]] const std::unique_ptr<std::byte[]> &GetData() const { return data_; }
  [[nodiscard]] std::unique_ptr<std::byte[]> &GetData() { return data_; }

private:
  std::unique_ptr<std::byte[]> data_;
  Vector2i size_;
  int32_t channels_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_CORE_IMAGE_H