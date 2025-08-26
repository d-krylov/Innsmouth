#include "key.h"
#include <utility>

namespace Innsmouth {

ImGuiKey ToImGuiKey(Key key) {
  switch (key) {
  case Key::K_TAB: return ImGuiKey_Tab;
  case Key::K_LEFT: return ImGuiKey_LeftArrow;
  case Key::K_RIGHT: return ImGuiKey_RightArrow;
  case Key::K_UP: return ImGuiKey_UpArrow;
  case Key::K_DOWN: return ImGuiKey_DownArrow;
  case Key::K_PAGE_UP: return ImGuiKey_PageUp;
  case Key::K_PAGE_DOWN: return ImGuiKey_PageDown;
  case Key::K_HOME: return ImGuiKey_Home;
  case Key::K_END: return ImGuiKey_End;
  case Key::K_INSERT: return ImGuiKey_Insert;
  case Key::K_DELETE: return ImGuiKey_Delete;
  case Key::K_BACKSPACE: return ImGuiKey_Backspace;
  case Key::K_SPACE: return ImGuiKey_Space;
  case Key::K_ENTER: return ImGuiKey_Enter;
  case Key::K_ESCAPE: return ImGuiKey_Escape;
  case Key::K_APOSTROPHE: return ImGuiKey_Apostrophe;
  case Key::K_COMMA: return ImGuiKey_Comma;
  case Key::K_MINUS: return ImGuiKey_Minus;
  case Key::K_PERIOD: return ImGuiKey_Period;
  case Key::K_SLASH: return ImGuiKey_Slash;
  case Key::K_SEMICOLON: return ImGuiKey_Semicolon;
  case Key::K_EQUAL: return ImGuiKey_Equal;
  case Key::K_LEFT_BRACKET: return ImGuiKey_LeftBracket;
  case Key::K_BACKSLASH: return ImGuiKey_Backslash;
  case Key::K_RIGHT_BRACKET: return ImGuiKey_RightBracket;
  case Key::K_GRAVE_ACCENT: return ImGuiKey_GraveAccent;
  case Key::K_0: return ImGuiKey_0;
  case Key::K_1: return ImGuiKey_1;
  case Key::K_2: return ImGuiKey_2;
  case Key::K_3: return ImGuiKey_3;
  case Key::K_4: return ImGuiKey_4;
  case Key::K_5: return ImGuiKey_5;
  case Key::K_6: return ImGuiKey_6;
  case Key::K_7: return ImGuiKey_7;
  case Key::K_8: return ImGuiKey_8;
  case Key::K_9: return ImGuiKey_9;
  case Key::K_A: return ImGuiKey_A;
  case Key::K_B: return ImGuiKey_B;
  case Key::K_C: return ImGuiKey_C;
  case Key::K_D: return ImGuiKey_D;
  case Key::K_E: return ImGuiKey_E;
  case Key::K_F: return ImGuiKey_F;
  case Key::K_G: return ImGuiKey_G;
  case Key::K_H: return ImGuiKey_H;
  case Key::K_I: return ImGuiKey_I;
  case Key::K_J: return ImGuiKey_J;
  case Key::K_K: return ImGuiKey_K;
  case Key::K_L: return ImGuiKey_L;
  case Key::K_M: return ImGuiKey_M;
  case Key::K_N: return ImGuiKey_N;
  case Key::K_O: return ImGuiKey_O;
  case Key::K_P: return ImGuiKey_P;
  case Key::K_Q: return ImGuiKey_Q;
  case Key::K_R: return ImGuiKey_R;
  case Key::K_S: return ImGuiKey_S;
  case Key::K_T: return ImGuiKey_T;
  case Key::K_U: return ImGuiKey_U;
  case Key::K_V: return ImGuiKey_V;
  case Key::K_W: return ImGuiKey_W;
  case Key::K_X: return ImGuiKey_X;
  case Key::K_Y: return ImGuiKey_Y;
  case Key::K_Z: return ImGuiKey_Z;
  case Key::K_F1: return ImGuiKey_F1;
  case Key::K_F2: return ImGuiKey_F2;
  case Key::K_F3: return ImGuiKey_F3;
  case Key::K_F4: return ImGuiKey_F4;
  case Key::K_F5: return ImGuiKey_F5;
  case Key::K_F6: return ImGuiKey_F6;
  case Key::K_F7: return ImGuiKey_F7;
  case Key::K_F8: return ImGuiKey_F8;
  case Key::K_F9: return ImGuiKey_F9;
  case Key::K_F10: return ImGuiKey_F10;
  case Key::K_F11: return ImGuiKey_F11;
  case Key::K_F12: return ImGuiKey_F12;
  case Key::K_KP_0: return ImGuiKey_Keypad0;
  case Key::K_KP_1: return ImGuiKey_Keypad1;
  case Key::K_KP_2: return ImGuiKey_Keypad2;
  case Key::K_KP_3: return ImGuiKey_Keypad3;
  case Key::K_KP_4: return ImGuiKey_Keypad4;
  case Key::K_KP_5: return ImGuiKey_Keypad5;
  case Key::K_KP_6: return ImGuiKey_Keypad6;
  case Key::K_KP_7: return ImGuiKey_Keypad7;
  case Key::K_KP_8: return ImGuiKey_Keypad8;
  case Key::K_KP_9: return ImGuiKey_Keypad9;
  case Key::K_KP_DECIMAL: return ImGuiKey_KeypadDecimal;
  case Key::K_KP_DIVIDE: return ImGuiKey_KeypadDivide;
  case Key::K_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
  case Key::K_KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
  case Key::K_KP_ADD: return ImGuiKey_KeypadAdd;
  case Key::K_KP_ENTER: return ImGuiKey_KeypadEnter;
  case Key::K_KP_EQUAL: return ImGuiKey_KeypadEqual;
  case Key::K_LEFT_SHIFT: return ImGuiKey_LeftShift;
  case Key::K_LEFT_CONTROL: return ImGuiKey_LeftCtrl;
  case Key::K_LEFT_ALT: return ImGuiKey_LeftAlt;
  case Key::K_LEFT_SUPER: return ImGuiKey_LeftSuper;
  case Key::K_RIGHT_SHIFT: return ImGuiKey_RightShift;
  case Key::K_RIGHT_CONTROL: return ImGuiKey_RightCtrl;
  case Key::K_RIGHT_ALT: return ImGuiKey_RightAlt;
  case Key::K_RIGHT_SUPER: return ImGuiKey_RightSuper;
  case Key::K_MENU: return ImGuiKey_Menu;
  default: break;
  }
  return ImGuiKey_None;
}

} // namespace Innsmouth