#include "innsmouth/scene/include/camera.h"

namespace Innsmouth {

Camera::Camera(const Vector3f position, const Vector3f &direction) : position_(position), direction_(direction) {
}

Matrix4f Camera::GetViewMatrix() const {
  return glm::lookAt(position_, position_ + direction_, up_);
}

Matrix4f Camera::GetProjectionMatrix() const {
  return glm::perspective(fov_, aspect_, near_, far_);
}

void Camera::SetPosition(const Vector3f &position) {
  position_ = position;
}

const Vector3f &Camera::GetPosition() const {
  return position_;
}

float Camera::GetFOV() const {
  return fov_;
}

void Camera::SetFOV(float fov) {
  fov_ = fov;
}

void Camera::SetAspect(float aspect) {
  aspect_ = aspect;
}

} // namespace Innsmouth