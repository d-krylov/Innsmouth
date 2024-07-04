#include "scene/include/camera.h"

namespace Innsmouth {

Camera::Camera() { UpdateVectors(); }

Matrix4f Camera::GetPerspectiveMatrix() const {
  return glm::perspective(fov_, aspect_, near_, far_);
}

Matrix4f Camera::GetLookAtMatrix() const { return glm::lookAt(position_, position_ + front_, up_); }

void Camera::UpdateVectors() {
  Vector3f front;
  front.x = glm::cos(yaw_) * glm::cos(pitch_);
  front.y = glm::sin(pitch_);
  front.z = glm::sin(yaw_) * glm::cos(pitch_);
  front_ = glm::normalize(front);
  right_ = glm::normalize(glm::cross(front_, world_up_));
  up_ = glm::normalize(glm::cross(right_, front_));
}

void Camera::SetPosition(const Vector3f &position) { position_ = position; }
void Camera::SetAspect(float aspect) { aspect_ = aspect; }

void Camera::SetYaw(float value) {
  yaw_ = value;
  UpdateVectors();
}

void Camera::SetPitch(float value) {
  pitch_ = value;
  UpdateVectors();
}

void Camera::MoveRight(float v) {}

} // namespace Innsmouth