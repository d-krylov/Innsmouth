#include "innsmouth/gui/include/key.h"
#include "innsmouth/core/include/macros.h"
#include <GLFW/glfw3.h>

namespace Innsmouth {

Key ToKey(int32_t key) {
  switch (key) {
#define KEY(X, Y)                                                                                  \
  case GLFW_KEY_##X:                                                                               \
    return Key::K_##X;
#include "innsmouth/gui/include/key.def"
  default:
    CORE_UNREACHABLE();
    break;
  }
}

int32_t ToNativeKey(Key key) {
  switch (key) {
#define KEY(X, Y)                                                                                  \
  case Key::K_##X:                                                                                 \
    return GLFW_KEY_##X;
#include "innsmouth/gui/include/key.def"
  default:
    CORE_UNREACHABLE();
    break;
  }
}

MouseButton ToMouseButton(int32_t button) {
  switch (button) {
  case GLFW_MOUSE_BUTTON_1:
    return MouseButton::BUTTON_1;
  case GLFW_MOUSE_BUTTON_2:
    return MouseButton::BUTTON_2;
  case GLFW_MOUSE_BUTTON_3:
    return MouseButton::BUTTON_3;
  case GLFW_MOUSE_BUTTON_4:
    return MouseButton::BUTTON_4;
  case GLFW_MOUSE_BUTTON_5:
    return MouseButton::BUTTON_5;
  case GLFW_MOUSE_BUTTON_6:
    return MouseButton::BUTTON_6;
  case GLFW_MOUSE_BUTTON_7:
    return MouseButton::BUTTON_7;
  case GLFW_MOUSE_BUTTON_8:
    return MouseButton::BUTTON_8;
  default:
    CORE_UNREACHABLE();
    break;
  }
}

Action ToAction(int32_t action) {
  switch (action) {
  case GLFW_PRESS:
    return Action::PRESS;
  case GLFW_RELEASE:
    return Action::RELEASE;
  case GLFW_REPEAT:
    return Action::REPEAT;
  default:
    CORE_UNREACHABLE();
    break;
  }
}

} // namespace Innsmouth