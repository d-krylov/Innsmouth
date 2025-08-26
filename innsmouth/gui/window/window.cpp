#include "window.h"
#include "mouse_events.h"
#include "key_events.h"
#include "window_events.h"
#include <GLFW/glfw3.h>

namespace Innsmouth {

Window *GetWindow(GLFWwindow *native_window) {
  return reinterpret_cast<Window *>(glfwGetWindowUserPointer(native_window));
}

void KeyCallback(GLFWwindow *native_window, int32_t key, int32_t scan, int32_t action, int32_t m) {
  auto *window = GetWindow(native_window);
  auto event = KeyEvent(Key(key), scan, Action(action));
  window->Invoke(event);
}

void CharCallback(GLFWwindow *native_window, uint32_t character) {
  auto *window = GetWindow(native_window);
  auto event = CharEvent(character);
  window->Invoke(event);
}

void CursorPositionCallback(GLFWwindow *native_window, double x, double y) {
  auto *window = GetWindow(native_window);
  auto event = MouseMoveEvent((float)x, (float)y);
  window->Invoke(event);
}

void ScrollCallback(GLFWwindow *native_window, double x, double y) {
  auto *window = GetWindow(native_window);
  auto event = MouseScrollEvent((float)x, (float)y);
  window->Invoke(event);
}

void MouseButtonCallback(GLFWwindow *native_window, int32_t button, int32_t action, int32_t mods) {
  auto *window = GetWindow(native_window);
  auto event = MouseButtonEvent(MouseButton(button), Action(action));
  window->Invoke(event);
}

void WindowSizeCallback(GLFWwindow *native_window, int32_t width, int32_t height) {
  auto *window = GetWindow(native_window);
  auto event = WindowResizeEvent(width, height);
  window->Invoke(event);
}

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

void SetCallbacks(GLFWwindow *native_window) {
  glfwSetWindowPosCallback(native_window, WindowPosCallback);
  glfwSetWindowFocusCallback(native_window, WindowFocusCallback);
  glfwSetKeyCallback(native_window, KeyCallback);
  glfwSetWindowSizeCallback(native_window, WindowSizeCallback);
  glfwSetWindowCloseCallback(native_window, WindowCloseCallback);
  glfwSetWindowRefreshCallback(native_window, WindowRefreshCallback);
  glfwSetCursorPosCallback(native_window, CursorPositionCallback);
  glfwSetCursorEnterCallback(native_window, CursorEnterCallback);
  glfwSetMouseButtonCallback(native_window, MouseButtonCallback);
  glfwSetScrollCallback(native_window, ScrollCallback);
  glfwSetCharCallback(native_window, CharCallback);
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

  SetCallbacks(GetNativeWindow());
}

Extent2D Window::GetSize() const {
  Extent2D size;
  glfwGetWindowSize(native_window_, &size.width, &size.height);
  return size;
}

Extent2D Window::GetFramebufferSize() const {
  Extent2D size;
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

int32_t Window::GetKey(int32_t key) const {
  return glfwGetKey(native_window_, key);
}

float Window::GetAspect() const {
  auto size = GetSize();
  auto width = static_cast<float>(size.width);
  auto height = static_cast<float>(size.height);
  return width / height;
}

void Window::Invoke(Event &event) {
  if (event_handler_) {
    event_handler_(event);
  }
}

void Window::SetEventHandler(const EventHandler &handler) {
  event_handler_ = handler;
}

} // namespace Innsmouth