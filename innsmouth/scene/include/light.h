#ifndef INNSMOUTH_LIGHT_H
#define INNSMOUTH_LIGHT_H

#include "innsmouth/core/include/core_types.h"

namespace Innsmouth {

class PointLight {
public:
  Vector3f position_{0.0f, 0.0f, 0.0f};
  Vector3f color_{0.0f, 0.0f, 0.0f};
  Vector3f attenuation_{0.0f, 0.0f, 1.0f};
};

} // namespace Innsmouth

#endif // INNSMOUTH_LIGHT_H