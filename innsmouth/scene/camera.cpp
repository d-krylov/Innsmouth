#include "innsmouth/scene/include/camera.h"

namespace Innsmouth {

Camera::Camera() { UpdateVectors(); }

Matrix4f Camera::GetPerspectiveMatrix() const { return glm::perspective(fov_, aspect_, near_, far_); }

Matrix4f Camera::GetLookAtMatrix() const { return glm::lookAt(position_, position_ + front_, up_); }

void Camera::UpdateVectors() {
  auto x = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_.x), X_);
  auto y = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_.y), Y_);
  auto z = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_.z), Z_);
  auto R = x * y * z;
  front_ = Vector3f(R * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
  right_ = Vector3f(R * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
  up_ = Vector3f(R * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
}

void Camera::SetPosition(const Vector3f &position) { position_ = position; }
void Camera::SetAspect(float aspect) { aspect_ = aspect; }

void Camera::SetRotation(const Vector3f &rotation) {
  rotation_ = rotation;
  UpdateVectors();
}

} // namespace Innsmouth