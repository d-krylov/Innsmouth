#include "key.h"
#include <utility>

namespace Innsmouth {

// clang-format off
Key ImGuiKeyToKey(ImGuiKey imgui_key) {
  switch (imgui_key) {
  case ImGuiKey_None: return Key::K_NONE;
  case ImGuiKey_Tab: return Key::K_TAB;
  case ImGuiKey_LeftArrow: return Key::K_LEFT_ARROW;
  case ImGuiKey_RightArrow: return Key::K_RIGHT_ARROW;
  case ImGuiKey_UpArrow: return Key::K_UP_ARROW;
  case ImGuiKey_DownArrow: return Key::K_DOWN_ARROW;
  case ImGuiKey_PageUp: return Key::K_PAGE_UP;
  case ImGuiKey_PageDown: return Key::K_PAGE_DOWN;
  case ImGuiKey_Home: return Key::K_HOME;
  case ImGuiKey_End: return Key::K_END;
  case ImGuiKey_Insert: return Key::K_INSERT;
  case ImGuiKey_Delete: return Key::K_DELETE;
  case ImGuiKey_Backspace: return Key::K_BACKSPACE;
  case ImGuiKey_Space: return Key::K_SPACE;
  case ImGuiKey_Enter: return Key::K_ENTER;
  case ImGuiKey_Escape: return Key::K_ESCAPE;
  case ImGuiKey_LeftCtrl: return Key::K_LEFT_CTRL;
  case ImGuiKey_LeftShift: return Key::K_LEFT_SHIFT;
  case ImGuiKey_LeftAlt: return Key::K_LEFT_ALT;
  case ImGuiKey_LeftSuper: return Key::K_LEFT_SUPER;
  case ImGuiKey_RightCtrl: return Key::K_RIGHT_CTRL;
  case ImGuiKey_RightShift: return Key::K_RIGHT_SHIFT;
  case ImGuiKey_RightAlt: return Key::K_RIGHT_ALT;
  case ImGuiKey_RightSuper: return Key::K_RIGHT_SUPER;
  case ImGuiKey_Menu: return Key::K_MENU;
  case ImGuiKey_0: return Key::K_0;
  case ImGuiKey_1: return Key::K_1;
  case ImGuiKey_2: return Key::K_2;
  case ImGuiKey_3: return Key::K_3;
  case ImGuiKey_4: return Key::K_4;
  case ImGuiKey_5: return Key::K_5;
  case ImGuiKey_6: return Key::K_6;
  case ImGuiKey_7: return Key::K_7;
  case ImGuiKey_8: return Key::K_8;
  case ImGuiKey_9: return Key::K_9;
  case ImGuiKey_A: return Key::K_A;
  case ImGuiKey_B: return Key::K_B;
  case ImGuiKey_C: return Key::K_C;
  case ImGuiKey_D: return Key::K_D;
  case ImGuiKey_E: return Key::K_E;
  case ImGuiKey_F: return Key::K_F;
  case ImGuiKey_G: return Key::K_G;
  case ImGuiKey_H: return Key::K_H;
  case ImGuiKey_I: return Key::K_I;
  case ImGuiKey_J: return Key::K_J;
  case ImGuiKey_K:
  case ImGuiKey_L:
  case ImGuiKey_M:
  case ImGuiKey_N:
  case ImGuiKey_O:
  case ImGuiKey_P:
  case ImGuiKey_Q:
  case ImGuiKey_R:
  case ImGuiKey_S:
  case ImGuiKey_T:
  case ImGuiKey_U:
  case ImGuiKey_V:
  case ImGuiKey_W:
  case ImGuiKey_X:
  case ImGuiKey_Y:
  case ImGuiKey_Z:
  case ImGuiKey_F1:
  case ImGuiKey_F2:
  case ImGuiKey_F3:
  case ImGuiKey_F4:
  case ImGuiKey_F5:
  case ImGuiKey_F6:
  case ImGuiKey_F7:
  case ImGuiKey_F8:
  case ImGuiKey_F9:
  case ImGuiKey_F10:
  case ImGuiKey_F11:
  case ImGuiKey_F12:
  case ImGuiKey_F13:
  case ImGuiKey_F14:
  case ImGuiKey_F15:
  case ImGuiKey_F16:
  case ImGuiKey_F17:
  case ImGuiKey_F18:
  case ImGuiKey_F19:
  case ImGuiKey_F20:
  case ImGuiKey_F21:
  case ImGuiKey_F22:
  case ImGuiKey_F23:
  case ImGuiKey_F24:
  case ImGuiKey_Apostrophe:
  case ImGuiKey_Comma:
  case ImGuiKey_Minus:
  case ImGuiKey_Period:
  case ImGuiKey_Slash:
  case ImGuiKey_Semicolon:
  case ImGuiKey_Equal:
  case ImGuiKey_LeftBracket:
  case ImGuiKey_Backslash:
  case ImGuiKey_RightBracket:
  case ImGuiKey_GraveAccent:
  case ImGuiKey_CapsLock:
  case ImGuiKey_ScrollLock:
  case ImGuiKey_NumLock:
  case ImGuiKey_PrintScreen:
  case ImGuiKey_Pause:
  case ImGuiKey_Keypad0:
  case ImGuiKey_Keypad1:
  case ImGuiKey_Keypad2:
  case ImGuiKey_Keypad3:
  case ImGuiKey_Keypad4:
  case ImGuiKey_Keypad5:
  case ImGuiKey_Keypad6:
  case ImGuiKey_Keypad7:
  case ImGuiKey_Keypad8:
  case ImGuiKey_Keypad9:
  case ImGuiKey_KeypadDecimal:
  case ImGuiKey_KeypadDivide:
  case ImGuiKey_KeypadMultiply:
  case ImGuiKey_KeypadSubtract:
  case ImGuiKey_KeypadAdd:
  case ImGuiKey_KeypadEnter:
  case ImGuiKey_KeypadEqual:
  case ImGuiKey_AppBack:
  case ImGuiKey_AppForward:
  case ImGuiKey_GamepadStart:
  case ImGuiKey_GamepadBack:
  case ImGuiKey_GamepadFaceLeft:
  case ImGuiKey_GamepadFaceRight:
  case ImGuiKey_GamepadFaceUp:
  case ImGuiKey_GamepadFaceDown:
  case ImGuiKey_GamepadDpadLeft:
  case ImGuiKey_GamepadDpadRight:
  case ImGuiKey_GamepadDpadUp:
  case ImGuiKey_GamepadDpadDown:
  case ImGuiKey_GamepadL1:
  case ImGuiKey_GamepadR1:
  case ImGuiKey_GamepadL2:
  case ImGuiKey_GamepadR2:
  case ImGuiKey_GamepadL3:
  case ImGuiKey_GamepadR3:
  case ImGuiKey_GamepadLStickLeft:
  case ImGuiKey_GamepadLStickRight:
  case ImGuiKey_GamepadLStickUp:
  case ImGuiKey_GamepadLStickDown:
  case ImGuiKey_GamepadRStickLeft:
  case ImGuiKey_GamepadRStickRight:
  case ImGuiKey_GamepadRStickUp:
  case ImGuiKey_GamepadRStickDown:
  case ImGuiKey_MouseLeft:
  case ImGuiKey_MouseRight:
  case ImGuiKey_MouseMiddle:
  case ImGuiKey_MouseX1:
  case ImGuiKey_MouseX2:
  case ImGuiKey_MouseWheelX:
  case ImGuiKey_MouseWheelY:
  case ImGuiKey_ReservedForModCtrl:
  case ImGuiKey_ReservedForModShift:
  case ImGuiKey_ReservedForModAlt:
  case ImGuiKey_ReservedForModSuper:
  case ImGuiKey_NamedKey_END: 
  case ImGuiMod_Ctrl: return Key::K_MOD_CTRL;
  case ImGuiMod_Shift: return Key::K_MOD_SHIFT;
  case ImGuiMod_Alt: return Key::K_MOD_ALT;
  case ImGuiMod_Super: return Key::K_MOD_SUPER;
  default: break;
  }
}
// clang-format on

} // namespace Innsmouth