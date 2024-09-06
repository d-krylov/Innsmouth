#include "innsmouth/widgets/include/transform_widget.h"
#include "imgui.h"

namespace Innsmouth {

void TransformWidget::OnImGui() {
  ImGui::Begin("Transform");
  ImGui::InputFloat3("Translate", glm::value_ptr(transform_.translate_));
  ImGui::SliderFloat3("Rotate   ", glm::value_ptr(transform_.rotate_), 0.0f, 6.28f);
  ImGui::InputFloat3("Scale    ", glm::value_ptr(transform_.scale_));
  ImGui::End();
}

} // namespace Innsmouth