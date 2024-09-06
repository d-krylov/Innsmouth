#include "innsmouth/gui/include/window.h"
#include "innsmouth/graphics/include/graphics.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Innsmouth {

Window::Window(std::string_view name, const Vector2i &size) {
  auto status = glfwInit();

  if (status != GLFW_TRUE) {
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  native_window_ = glfwCreateWindow(size.x, size.y, name.data(), nullptr, nullptr);

  if (native_window_ == nullptr) {
  }

  SetUserPointer();

  SetCallbacks();
}

void Window::SetCursor(int32_t index) {}

void Window::PollEvents() { glfwPollEvents(); }

bool Window::ShouldClose() const { return glfwWindowShouldClose(native_window_); }

void Window::CreateSurface(const VkInstance instance, VkSurfaceKHR *surface) {
  VK_CHECK(glfwCreateWindowSurface(instance, native_window_, nullptr, surface));
}

Vector2i Window::GetFramebufferSize() const {
  Vector2i size;
  glfwGetFramebufferSize(native_window_, &size.x, &size.y);
  return size;
}

Vector2i Window::GetSize() const {
  Vector2i size;
  glfwGetWindowSize(native_window_, &size.x, &size.y);
  return size;
}

int32_t Window::GetWidth() const { return GetSize().x; }
int32_t Window::GetHeight() const { return GetSize().y; }

Vector2i Window::GetPosition() const {
  Vector2i position;
  glfwGetWindowPos(native_window_, &position.x, &position.y);
  return position;
}

Vector2f Window::GetCursorPosition() const {
  Vector2d position;
  glfwGetCursorPos(native_window_, &position.x, &position.y);
  return Vector2f(position);
}

} // namespace Innsmouth