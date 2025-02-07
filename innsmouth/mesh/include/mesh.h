#ifndef INNSMOUTH_MESH_H
#define INNSMOUTH_MESH_H

#include "mathematics/include/mathematical_types.h"

namespace Innsmouth {

struct PlainVertex {
  Vector3f position_;
  Vector3f normal_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_MESH_H