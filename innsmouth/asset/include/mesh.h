#ifndef INNSMOUTH_MESH_H
#define INNSMOUTH_MESH_H

#include "innsmouth/mathematics/include/mathematics_types.h"

namespace Innsmouth {

struct Vertex {
  Vector3f position_;
  Vector3f normal_;
  Vector2f uv_;
};

struct Mesh {
  int32_t color_texture_index;
  int32_t normal_texture_index;
  uint32_t vertices_offset;
  uint32_t indices_offset;
  uint32_t indices_size;
};

} // namespace Innsmouth

#endif // INNSMOUTH_MESH_H