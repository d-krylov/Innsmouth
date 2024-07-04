#ifndef INNSMOUTH_CAMERA_H
#define INNSMOUTH_CAMERA_H

#include "core/include/core_types.h"

namespace Innsmouth {

class Camera {
public:
  Camera();

  [[nodiscard]] const Vector3f &GetUpDirection() const { return up_; }
  [[nodiscard]] const Vector3f &GetRightDirection() const { return right_; }
  [[nodiscard]] const Vector3f &GetFrontDirection() const { return front_; }
  [[nodiscard]] const Vector3f &GetPosition() { return position_; }

  [[nodiscard]] float GetYaw() const { return yaw_; }
  [[nodiscard]] float GetPitch() const { return pitch_; }

  [[nodiscard]] Matrix4f GetPerspectiveMatrix() const;
  [[nodiscard]] Matrix4f GetLookAtMatrix() const;

  void MoveUp(float v);
  void MoveRight(float v);

  void SetPosition(const Vector3f &position);
  void SetYaw(float value);
  void SetPitch(float value);

  void SetAspect(float aspect);

protected:
  void UpdateVectors();

private:
  Vector3f position_{0.0f, 0.0f, 0.0f};
  Vector3f front_{0.0f, 0.0f, -1.0f};
  Vector3f right_;
  Vector3f up_;
  Vector3f world_up_{Y_};
  float fov_{PI_ / 3.0f};
  float near_{0.1f};
  float far_{1000.0f};
  float aspect_{1.7f};
  float pitch_{0.0f};
  float yaw_{-PI_ / 2.0f};
  float speed_{0.5f};
};

} // namespace Innsmouth

#endif // INNSMOUTH_CAMERA_H