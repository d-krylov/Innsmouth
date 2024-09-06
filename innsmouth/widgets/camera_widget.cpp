#include "innsmouth/widgets/include/camera_widget.h"
#include "imgui.h"

namespace Innsmouth {

void CameraWidget::OnImGui() {
  position_ = camera_.GetPosition();
  rotation_ = camera_.GetRotation();
  ImGui::Begin("Camera");
  ImGui::InputFloat3("camera position", glm::value_ptr(position_));
  ImGui::SliderFloat3("camera rotation", glm::value_ptr(rotation_), 0.0f, 360.0f);
  ImGui::End();
}

void CameraWidget::OnUpdate() {
  camera_.SetPosition(position_);
  camera_.SetRotation(rotation_);
}

} // namespace Innsmouth