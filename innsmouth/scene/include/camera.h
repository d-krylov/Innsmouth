#ifndef INNSMOUTH_CAMERA_H
#define INNSMOUTH_CAMERA_H

#include "innsmouth/mathematics/include/mathematics_types.h"

namespace Innsmouth {

class Camera {
public:
  Camera(const Vector3f position = Vector3f(0.0f), const Vector3f &front = Vector3f(0.0f, 0.0f, 1.0f));

  const Vector3f &GetPosition() const;

  float GetFOV() const;

  float GetYaw() const;
  float GetPitch() const;

  void SetYaw(float yaw);
  void SetPitch(float pitch);

  Matrix4f GetViewMatrix() const;
  Matrix4f GetProjectionMatrix() const;

  void SetPosition(const Vector3f &position);
  void SetAspect(float aspect);
  void SetFOV(float fov);

protected:
  void UpdateCameraVectors();

private:
  Vector3f position_;
  Vector3f front_;
  Vector3f right_;
  Vector3f up_ = Vector3f(0.0, 1.0f, 0.0f);
  float aspect_ = 1.0f;
  float near_ = 0.1f;
  float far_ = 10000.0f;
  float fov_ = 1.0f;
  float yaw_ = -90.0f;
  float pitch_ = 0.0f;
};

} // namespace Innsmouth

#endif // INNSMOUTH_CAMERA_H