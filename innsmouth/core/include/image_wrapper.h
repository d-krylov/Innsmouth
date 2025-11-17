#ifndef INNSMOUTH_IMAGE_WRAPPER_H
#define INNSMOUTH_IMAGE_WRAPPER_H

#include <filesystem>

namespace Innsmouth {

class ImageWrapper {
public:
  ImageWrapper(const std::filesystem::path &image_path);

  ~ImageWrapper();

private:
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_WRAPPER_H