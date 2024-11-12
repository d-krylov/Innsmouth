#include "mesh/include/transform.h"

namespace Innsmouth {

Matrix4f Transform::GetModelMatrix() const {
  Matrix4f transform = Matrix4f(1.0f);
  transform = glm::translate(transform, translate_);
  transform = glm::rotate(transform, rotate_.x, X_);
  transform = glm::rotate(transform, rotate_.y, Y_);
  transform = glm::rotate(transform, rotate_.z, Z_);
  transform = glm::scale(transform, scale_);
  return transform;
}

} // namespace Innsmouth
