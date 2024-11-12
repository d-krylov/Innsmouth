#include "widgets/include/light_widget.h"
#include "imgui.h"
#include <string>

namespace Innsmouth {

void LightWidget::OnImGui() {
  ImGui::Begin("Lights");
  ImGui::InputFloat3("Position   ", glm::value_ptr(point_light_.position_));
  ImGui::InputFloat3("Color      ", glm::value_ptr(point_light_.color_));
  ImGui::InputFloat3("Attenuation", glm::value_ptr(point_light_.attenuation_));
  ImGui::End();
}

} // namespace Innsmouth