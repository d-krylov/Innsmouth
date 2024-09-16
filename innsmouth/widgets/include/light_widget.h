#ifndef INNSMOUTH_LIGHT_WIDGET_H
#define INNSMOUTH_LIGHT_WIDGET_H

#include "innsmouth/scene/include/light.h"
#include <vector>

namespace Innsmouth {

class LightWidget {
public:
  void OnImGui();

  [[nodiscard]] const PointLight &GetPointLight() const { return point_light_; }

private:
  PointLight point_light_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_LIGHT_WIDGET_H