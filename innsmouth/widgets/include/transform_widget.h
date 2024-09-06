#ifndef INNSMOUTH_TRANSFORM_WIDGET_H
#define INNSMOUTH_TRANSFORM_WIDGET_H

#include "innsmouth/mesh/include/transform.h"

namespace Innsmouth {

class TransformWidget {
public:
  void OnImGui();

  const Transform &GetTransform() const { return transform_; }

private:
  Transform transform_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_TRANSFORM_WIDGET_H