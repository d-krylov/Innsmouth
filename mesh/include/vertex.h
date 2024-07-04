#ifndef INNSMOUTH_VERTEX_H
#define INNSMOUTH_VERTEX_H

#include "core/include/core_types.h"

namespace Innsmouth {

struct Vertex {
  Vector3f position_;
  Vector3f normal_;
  Vector2f uv_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_VERTEX_H