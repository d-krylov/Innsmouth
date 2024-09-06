#ifndef INNSMOUTH_CAMERA_WIDGET_H
#define INNSMOUTH_CAMERA_WIDGET_H

#include "innsmouth/scene/include/camera.h"

namespace Innsmouth {

class CameraWidget {
public:
  void OnImGui();
  void OnUpdate();

  [[nodiscard]] const Camera &GetCamera() const { return camera_; }
  [[nodiscard]] Camera &GetCamera() { return camera_; }

private:
  Vector3f position_;
  Vector3f rotation_;
  Camera camera_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_CAMERA_WIDGET_H