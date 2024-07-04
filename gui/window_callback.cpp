#include "gui/include/window.h"
#include <GLFW/glfw3.h>

namespace Innsmouth {

void KeyCallback(GLFWwindow *native, int key, int scancode, int action, int mod) {
  Window *window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(native));
  const auto &callback = window->GetEventCallback();
  switch (action) {
  case GLFW_PRESS: {
    KeyPressEvent event(key, scancode, mod);
    callback(event);
    break;
  }
  case GLFW_RELEASE: {
    KeyReleaseEvent event(key, scancode, mod);
    callback(event);
    break;
  }
  case GLFW_REPEAT: {
    KeyRepeatEvent event(key, scancode, mod);
    callback(event);
    break;
  }
  default:
    break;
  }
}

void WindowSizeCallback(GLFWwindow *wnd, int width, int height) {}

void WindowCloseCallback(GLFWwindow *wnd) {}

void CursorPositionCallback(GLFWwindow *wnd, double xpos, double ypos) {}

void MouseButtonCallback(GLFWwindow *wnd, int button, int action, int mods) {}

void WindowRefreshCallback(GLFWwindow *wnd) {}

void ScrollCallback(GLFWwindow *wnd, double xoffset, double yoffset) {}

void CursorEnterCallback(GLFWwindow *wnd, int entered) {}

void WindowFocusCallback(GLFWwindow *window, int focused) {}

void MonitorCallback(GLFWmonitor *, int) {}

void CharCallback(GLFWwindow *window, unsigned int c) {}

void WindowPosCallback(GLFWwindow *window, int, int) {}

void Window::SetUserPointer() { glfwSetWindowUserPointer(native_window_, this); }

void Window::SetCallbacks() {
  glfwSetWindowPosCallback(native_window_, WindowPosCallback);
  glfwSetWindowFocusCallback(native_window_, WindowFocusCallback);
  glfwSetKeyCallback(native_window_, KeyCallback);
  glfwSetWindowSizeCallback(native_window_, WindowSizeCallback);
  glfwSetWindowCloseCallback(native_window_, WindowCloseCallback);
  glfwSetWindowRefreshCallback(native_window_, WindowRefreshCallback);
  glfwSetCursorPosCallback(native_window_, CursorPositionCallback);
  glfwSetCursorEnterCallback(native_window_, CursorEnterCallback);
  glfwSetMouseButtonCallback(native_window_, MouseButtonCallback);
  glfwSetScrollCallback(native_window_, ScrollCallback);
  glfwSetCharCallback(native_window_, CharCallback);
  glfwSetMonitorCallback(MonitorCallback);
}

} // namespace Innsmouth