#ifndef INNSMOUTH_MATERIAL_H
#define INNSMOUTH_MATERIAL_H

#include "innsmouth/core/include/core_types.h"
#include "innsmouth/graphics/image/image_2d.h"
#include <memory>

namespace Innsmouth {

enum TextureType { AMBIENT = 0, DIFFUSE, SPECULAR, BUMP, COUNT };

struct Material {
  float shininess;
  std::array<std::string, TextureType::COUNT> textures_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_MATERIAL_H