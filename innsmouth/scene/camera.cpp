#include "innsmouth/scene/include/camera.h"

namespace Innsmouth {

Camera::Camera(const Vector3f position, const Vector3f &front) : position_(position), front_(front) {
  UpdateCameraVectors();
}

Matrix4f Camera::GetViewMatrix() const {
  return glm::lookAt(position_, position_ + front_, up_);
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

float Camera::GetYaw() const {
  return yaw_;
}

float Camera::GetPitch() const {
  return pitch_;
}

void Camera::SetYaw(float yaw) {
  yaw_ = yaw;
  UpdateCameraVectors();
}

void Camera::SetPitch(float pitch) {
  pitch_ = pitch;
  UpdateCameraVectors();
}

void Camera::UpdateCameraVectors() {
  Vector3f front;
  front.x = cos(glm::radians(pitch_)) * cos(glm::radians(yaw_));
  front.y = sin(glm::radians(pitch_));
  front.z = cos(glm::radians(pitch_)) * sin(glm::radians(yaw_));
  front_ = glm::normalize(front);
  right_ = glm::normalize(glm::cross(front_, Vector3f(0.0f, 1.0f, 0.0f)));
  up_ = glm::normalize(glm::cross(right_, front_));
}

} // namespace Innsmouth