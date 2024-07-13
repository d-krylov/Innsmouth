#ifndef INNSMOUTH_MATERIAL_H
#define INNSMOUTH_MATERIAL_H

#include "core/include/core_types.h"
#include "graphics/image/image_2d.h"
#include <memory>

namespace Innsmouth {

struct MaterialProperties {
  Vector3f ambient_;
  Vector3f diffuse_;
  Vector3f specular_;
  Vector3f transmittance_;
  Vector3f emission_;
  float shininess_;
  float ior_;
  float dissolve_;
};

class Material {
public:
public:
  std::shared_ptr<MaterialProperties> properties_;
  std::shared_ptr<Image2D> ambient_texture_;
  std::shared_ptr<Image2D> diffuse_texture_;
  std::shared_ptr<Image2D> specular_texture_;
};

struct PBRMaterialProperties {
  float roughness_;
  float metallic_;
  float sheen_;
  float clearcoat_thickness_;
  float clearcoat_roughness_;
  float anisotropy_;
  float anisotropy_rotation_;
};

class PBRMaterial {
public:
};

} // namespace Innsmouth

#endif // INNSMOUTH_MATERIAL_H