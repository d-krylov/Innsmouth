#include "innsmouth/widgets/include/light_widget.h"
#include "imgui.h"
#include <string>

namespace Innsmouth {

void LightWidget::OnImGui() {
  ImGui::Begin("Lights");

  for (auto i = 0; i < point_lights_.size(); i++) {
    std::string name = "Point Light " + std::to_string(i);
    if (ImGui::CollapsingHeader(name.c_str())) {
      ImGui::InputFloat3("Position   ", glm::value_ptr(point_lights_[i].position_));
      ImGui::InputFloat3("Color      ", glm::value_ptr(point_lights_[i].color_));
      ImGui::InputFloat3("Attenuation", glm::value_ptr(point_lights_[i].attenuation_));
    }
  }

  ImGui::End();
}

} // namespace Innsmouth