#ifndef INNSMOUTH_LIGHT_WIDGET_H
#define INNSMOUTH_LIGHT_WIDGET_H

#include "innsmouth/scene/include/light.h"
#include <vector>

namespace Innsmouth {

class LightWidget {
public:
  void OnImGui();

  void AddPointLight() { point_lights_.emplace_back(); }

  [[nodiscard]] const std::vector<PointLight> &GetPointLights() const { return point_lights_; }

private:
  std::vector<PointLight> point_lights_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_LIGHT_WIDGET_H