#ifndef INNSMOUTH_MESH_H
#define INNSMOUTH_MESH_H

#include "mathematics/include/mathematical_types.h"

namespace Innsmouth {

// clang-format off

template <typename T>
concept IsVertex = requires(T t) {
  { t.position_ } -> std::same_as<Vector3f &>;
  { t.normal_ } -> std::same_as<Vector3f &>;
};

template <typename T>
concept IsTexturedVertex = requires(T t) {
  { t.uv_ } -> std::same_as<Vector2f &>;
};

// clang-format on

struct PlainVertex {
  Vector3f position_;
  Vector3f normal_;
};

struct TexturedVertex {
  Vector3f position_;
  Vector3f normal_;
  Vector2f uv_;
};

enum class TextureType { AMBIENT, DIFFUSE, SPECULAR, BUMP, METALLIC, ROUGHNESS, SHEEN, EMISSIVE, NORMAL };

inline constexpr auto TEXTURE_COUNT = 1 + std::to_underlying(TextureType::NORMAL);

struct Material {
  Material() { texture_names_.fill("default"); }

  std::array<std::string, TEXTURE_COUNT> texture_names_;
};

struct Mesh {
  uint32_t offset_;
  uint32_t size_;
  Material material_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_MESH_H