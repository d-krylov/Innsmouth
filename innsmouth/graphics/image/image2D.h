#ifndef INNSMOUTH_IMAGE_2D_H
#define INNSMOUTH_IMAGE_2D_H

#include <filesystem>

namespace Innsmouth {

class Image2D {
public:
  Image2D(const std::filesystem::path &image_path);

  ~Image2D();

private:
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMAGE_2D_H