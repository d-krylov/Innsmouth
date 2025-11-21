#include "innsmouth/core/include/image_wrapper.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <print>

namespace Innsmouth {

ImageWrapper::ImageWrapper(const std::filesystem::path &image_path) {
  mapped_data_ = stbi_load(image_path.c_str(), &width_, &height_, &channels_, STBI_rgb_alpha);
}

ImageWrapper::~ImageWrapper() {
  stbi_image_free(mapped_data_);
}

int32_t ImageWrapper::GetWidth() const {
  return width_;
}

int32_t ImageWrapper::GetHeight() const {
  return height_;
}

std::size_t ImageWrapper::GetSize() const {
  return 4 * width_ * height_;
}

std::span<const std::byte> ImageWrapper::GetData() const {
  return std::as_bytes(std::span(mapped_data_, GetSize()));
}

} // namespace Innsmouth