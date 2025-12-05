#include "innsmouth/mathematics/include/transform.h"

namespace Innsmouth {

Transform::Transform(const Vector3f &position, const Vector3f &orientation, const Vector3f &scale)
  : position_(position), orientation_(orientation), scale_(scale) {
}

Matrix4f Transform::GetModelMatrix() {
  auto translate = glm::translate(Matrix4f(1.0f), position_);
  auto rotate = glm::yawPitchRoll(orientation_.y, orientation_.x, orientation_.z);
  auto scale = glm::scale(Matrix4f(1.0f), scale_);
  return translate * rotate * scale;
}

void Transform::SetPosition(const Vector3f &position) {
  position_ = position;
}

void Transform::SetScale(const Vector3f &scale) {
  scale_ = scale;
}

void Transform::SetOrientation(const Vector3f &orientation) {
  orientation_ = orientation;
}

} // namespace Innsmouth