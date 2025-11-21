#ifndef INNSMOUTH_IMAGE_WRAPPER_H
#define INNSMOUTH_IMAGE_WRAPPER_H

#include <filesystem>
#include <span>

namespace Innsmouth {

class ImageWrapper {
public:
  ImageWrapper(const std::filesystem::path &image_path);

  ~ImageWrapper();

  int32_t GetWidth() const;
  int32_t GetHeight() const;

  std::size_t GetSize() const;

  std::span<const std::byte> GetData() const;

private:
  int32_t width_{0};
  int32_t height_{0};
  int32_t channels_{0};
  uint8_t *mapped_data_{nullptr};
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_WRAPPER_H