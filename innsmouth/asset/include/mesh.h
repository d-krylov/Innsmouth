#ifndef INNSMOUTH_MESH_H
#define INNSMOUTH_MESH_H

#include "innsmouth/mathematics/include/mathematics_types.h"

namespace Innsmouth {

struct Vertex {
  Vector3f position_;
  Vector3f normal_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_MESH_H