#ifndef INNSMOUTH_CAMERA_H
#define INNSMOUTH_CAMERA_H

#include "mathematics/include/mathematical_types.h"

namespace Innsmouth {

class Camera {
public:
  Camera();

  Matrix4f GetLookAtMatrix() const;

  float GetNear() const { return near_; }
  float GetFar() const { return far_; }

  Matrix4f GetProjectionMatrix() const;

  void SetNear(float near) { near_ = near; }
  void SetFar(float far) { far_ = far; }
  void SetPosition(const Vector3f &position) { position_ = position; }
  void SetRotation(const Vector3f &rotation);

  const Vector3f &GetPosition() const { return position_; }

protected:
  void UpdateVectors();

private:
  Vector3f position_{0.0f, 0.0f, 0.0f};
  Vector3f front_{0.0f, 0.0f, -1.0f};
  Vector3f right_;
  Vector3f up_;
  Vector3f rotation_{0.0f, 0.0f, 0.0f};
  float near_{0.1f};
  float far_{10000.0f};
  float aspect_{1.0f};
  float fov_{60.0f};
};

} // namespace Innsmouth

#endif // INNSMOUTH_CAMERA_H