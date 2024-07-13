#ifndef INNSMOUTH_IMGUI_PLATFORM_H
#define INNSMOUTH_IMGUI_PLATFORM_H

#include "gui/include/window.h"

namespace Innsmouth {

class ImGuiPlatform : public WindowEventHandler {
public:
  ImGuiPlatform(Window *window);
  ~ImGuiPlatform();

  void SetFonts();
  void NewFrame();

  void UpdateMouseData();

  void OnWindowFocus(int focused) override;
  void OnWindowSize(int, int) override;
  void OnWindowPos(int, int) override;
  void OnMouseButton(int button, int action, int m) override;
  void OnScroll(double xoffset, double yoffset) override;
  void OnKey(int keycode, int scancode, int action, int m) override;
  void OnCursorPos(double x, double y) override;
  void OnCursorEnter(int entered) override;
  void OnChar(unsigned int c) override;

private:
  Window *window_{nullptr};
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMGUI_PLATFORM_H
