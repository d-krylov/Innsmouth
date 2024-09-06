#include "innsmouth/gui/include/event.h"
#include "innsmouth/gui/include/window.h"
#include <GLFW/glfw3.h>

namespace Innsmouth {

Window *GetWindow(GLFWwindow *native_window) {
  return reinterpret_cast<Window *>(glfwGetWindowUserPointer(native_window));
}

void KeyCallback(GLFWwindow *native_window, int32_t key, int32_t scan, int32_t action, int32_t m) {
  auto *window = GetWindow(native_window);
  auto &handler = window->GetEventCallback();
  KeyEvent event(ToKey(key), scan, ToAction(action));
  handler(event);
}

void CharCallback(GLFWwindow *native_window, uint32_t character) {
  auto *window = GetWindow(native_window);
  auto &handler = window->GetEventCallback();
  CharEvent event(character);
  handler(event);
}

void CursorPositionCallback(GLFWwindow *native_window, double x, double y) {
  auto *window = GetWindow(native_window);
  auto &handler = window->GetEventCallback();
  MouseMoveEvent event((float)x, (float)y);
  handler(event);
}

void ScrollCallback(GLFWwindow *native_window, double x, double y) {
  auto *window = GetWindow(native_window);
  auto &handler = window->GetEventCallback();
  MouseScrollEvent event((float)x, (float)y);
  handler(event);
}

void MouseButtonCallback(GLFWwindow *native_window, int32_t button, int32_t action, int32_t mods) {
  auto *window = GetWindow(native_window);
  auto &handler = window->GetEventCallback();
  MouseButtonEvent event(ToMouseButton(button), ToAction(action));
  handler(event);
}

void WindowSizeCallback(GLFWwindow *native_window, int32_t width, int32_t height) {
  auto *window = GetWindow(native_window);
  auto &handler = window->GetEventCallback();
  WindowSizeEvent event(width, height);
  handler(event);
}

void WindowCloseCallback(GLFWwindow *native_window) {}

void WindowRefreshCallback(GLFWwindow *native_window) {}

void CursorEnterCallback(GLFWwindow *native_window, int32_t entered) {}

void WindowFocusCallback(GLFWwindow *native_window, int32_t focused) {}

void MonitorCallback(GLFWmonitor *, int32_t) {}

void WindowPosCallback(GLFWwindow *window, int32_t, int32_t) {}

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