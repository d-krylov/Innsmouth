#ifndef MORRA_WINDOW_EVENT_HANDLER_H
#define MORRA_WINDOW_EVENT_HANDLER_H

namespace Innsmouth {

class WindowEventHandler {
public:
  virtual void OnWindowFocus(int focused) = 0;
  virtual void OnWindowSize(int, int) = 0;
  virtual void OnWindowPos(int, int) = 0;
  virtual void OnMouseButton(int button, int action, int m) = 0;
  virtual void OnScroll(double xoffset, double yoffset) = 0;
  virtual void OnKey(int keycode, int scancode, int action, int m) = 0;
  virtual void OnCursorPos(double x, double y) = 0;
  virtual void OnCursorEnter(int entered) = 0;
  virtual void OnChar(unsigned int c) = 0;
};

} // namespace Innsmouth

#endif // MORRA_WINDOW_EVENT_HANDLER_H