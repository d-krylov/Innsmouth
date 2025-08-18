#include "window.h"
#include <GLFW/glfw3.h>

namespace Innsmouth {

#if 0
void KeyCallback(GLFWwindow *native_window, int32_t key, int32_t scan, int32_t action, int32_t m) {
  auto *window = GetWindow(native_window);
  auto &handler = window->GetEventCallback();
  KeyEvent event(Key(key), scan, Action(action));
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
  auto event = MouseButtonEvent(MouseButton(button), Action(action));
  handler(event);
}

void WindowSizeCallback(GLFWwindow *native_window, int32_t width, int32_t height) {
  auto *window = GetWindow(native_window);
  auto &handler = window->GetEventCallback();
  WindowResizeEvent event(width, height);
  handler(event);
}
#endif

void WindowCloseCallback(GLFWwindow *native_window) {
}

void WindowRefreshCallback(GLFWwindow *native_window) {
}

void CursorEnterCallback(GLFWwindow *native_window, int32_t entered) {
}

void WindowFocusCallback(GLFWwindow *native_window, int32_t focused) {
}

void MonitorCallback(GLFWmonitor *, int32_t) {
}

void WindowPosCallback(GLFWwindow *window, int32_t, int32_t) {
}

void Window::SetCallbacks() {
  glfwSetWindowPosCallback(native_window_, WindowPosCallback);
  glfwSetWindowFocusCallback(native_window_, WindowFocusCallback);
  // glfwSetKeyCallback(native_window_, KeyCallback);
  // glfwSetWindowSizeCallback(native_window_, WindowSizeCallback);
  glfwSetWindowCloseCallback(native_window_, WindowCloseCallback);
  glfwSetWindowRefreshCallback(native_window_, WindowRefreshCallback);
  // glfwSetCursorPosCallback(native_window_, CursorPositionCallback);
  glfwSetCursorEnterCallback(native_window_, CursorEnterCallback);
  // glfwSetMouseButtonCallback(native_window_, MouseButtonCallback);
  // glfwSetScrollCallback(native_window_, ScrollCallback);
  // glfwSetCharCallback(native_window_, CharCallback);
  glfwSetMonitorCallback(MonitorCallback);
}

Window::Window(std::string_view name, int32_t width, int32_t height) {
  auto status = glfwInit();

  if (status != GLFW_TRUE) {
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  native_window_ = glfwCreateWindow(width, height, name.data(), nullptr, nullptr);

  if (native_window_ == nullptr) {
  }

  glfwSetWindowUserPointer(native_window_, this);

  SetCallbacks();
}

Window::Extent2i Window::GetSize() const {
  Extent2i size;
  glfwGetWindowSize(native_window_, &size.width, &size.height);
  return size;
}

Window::Extent2i Window::GetFramebufferSize() const {
  Extent2i size;
  glfwGetFramebufferSize(native_window_, &size.width, &size.height);
  return size;
}

int32_t Window::GetWidth() const {
  auto size = GetSize();
  return size.width;
}

int32_t Window::GetHeight() const {
  auto size = GetSize();
  return size.height;
}

bool Window::ShouldClose() const {
  return glfwWindowShouldClose(native_window_);
}

void Window::PollEvents() {
  glfwPollEvents();
}

} // namespace Innsmouth