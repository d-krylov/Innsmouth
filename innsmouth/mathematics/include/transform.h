#ifndef INNSMOUTH_TRANSFORM_H
#define INNSMOUTH_TRANSFORM_H

#include "mathematics_types.h"

namespace Innsmouth {

class Transform {
public:
  Transform(const Vector3f &position = Vector3f(0.0f), const Vector3f &orientation = Vector3f(0.0f), const Vector3f &scale = Vector3f(1.0f));

  Matrix4f GetModelMatrix();

  void SetOrientation(const Vector3f &orientation);
  void SetPosition(const Vector3f &position);
  void SetScale(const Vector3f &scale);

private:
  Vector3f position_;
  Vector3f orientation_;
  Vector3f scale_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_TRANSFORM_H