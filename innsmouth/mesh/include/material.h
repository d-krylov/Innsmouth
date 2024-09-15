#ifndef INNSMOUTH_MATERIAL_H
#define INNSMOUTH_MATERIAL_H

#include "innsmouth/core/include/core_types.h"
#include "innsmouth/graphics/image/image_2d.h"
#include <memory>

namespace Innsmouth {

struct TextureNames {
  std::string ambient_;
  std::string diffuse_;
  std::string specular_;
  std::string bump_;
};

struct Material {
  float shininess;
  TextureNames names_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_MATERIAL_H