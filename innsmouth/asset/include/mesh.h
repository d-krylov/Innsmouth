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
  uint32_t indices_offset;
  uint32_t indices_size;
};

} // namespace Innsmouth

#endif // INNSMOUTH_MESH_H