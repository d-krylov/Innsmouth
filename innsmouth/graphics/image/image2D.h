#ifndef INNSMOUTH_IMAGE_2D_H
#define INNSMOUTH_IMAGE_2D_H

#include <filesystem>
#include "image.h"

namespace Innsmouth {

class Image2D : public Image {
public:
  Image2D() = default;

  Image2D(uint32_t width, uint32_t height, Format format, ImageUsageMask optional_usage);

  Image2D(const std::filesystem::path &image_path, const std::optional<SamplerSpecification> &sampler_specification = std::nullopt);

  Image2D(uint32_t width, uint32_t height, std::span<const std::byte> data,
          const std::optional<SamplerSpecification> &sampler_specification = std::nullopt);

  Image2D(Image2D &&other) noexcept = default;

  Image2D &operator=(Image2D &&other) noexcept = default;

protected:
  void Create(uint32_t width, uint32_t height, Format format, ImageUsageMask usage_mask,
              const std::optional<SamplerSpecification> &sampler_specification);
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_2D_H