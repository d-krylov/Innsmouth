#ifndef INNSMOUTH_KEY_H
#define INNSMOUTH_KEY_H

#include <cstdint>

namespace Innsmouth {

enum class Key {
#define KEY(X, Y) K_##X,
#include "key.def"
};

enum class MouseButton {
  BUTTON_1 = 0,
  BUTTON_2 = 1,
  BUTTON_3 = 2,
  BUTTON_4 = 3,
  BUTTON_5 = 4,
  BUTTON_6 = 5,
  BUTTON_7 = 6,
  BUTTON_8 = 7,
  LAST = BUTTON_8,
  LEFT = BUTTON_1,
  RIGHT = BUTTON_2,
  MIDDLE = BUTTON_3
};

enum class Action { PRESS, RELEASE, REPEAT };

[[nodiscard]] Key ToKey(int32_t key);
[[nodiscard]] int32_t ToNativeKey(Key key);
[[nodiscard]] MouseButton ToMouseButton(int32_t button);
[[nodiscard]] Action ToAction(int32_t action);

} // namespace Innsmouth

#endif // INNSMOUTH_KEY_H