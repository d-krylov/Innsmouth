#ifndef INNSMOUTH_TRANSFORM_H
#define INNSMOUTH_TRANSFORM_H

#include "core/include/core_types.h"

namespace Innsmouth {

class Transform {
public:
  [[nodiscard]] Matrix4f GetModelMatrix() const;

public:
  Vector3f translate_{0.0f};
  Vector3f rotate_{0.0f};
  Vector3f scale_{1.0f};
};

} // namespace Innsmouth

#endif // INNSMOUTH_TRANSFORM_H