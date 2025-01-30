#include "gui/include/window.h"
#include <GLFW/glfw3.h>

namespace Innsmouth {

Window::Window(std::string_view name, uint32_t width, uint32_t height) {
  auto status = glfwInit();

  if (status != GLFW_TRUE) {
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  native_window_ = glfwCreateWindow(width, height, name.data(), nullptr, nullptr);

  if (native_window_ == nullptr) {
  }
}

bool Window::ShouldClose() const { return glfwWindowShouldClose(native_window_); }

Extent2i Window::GetSize() const {
  Extent2i size;
  glfwGetWindowSize(native_window_, &size.width, &size.height);
  return size;
}

Extent2i Window::GetFramebufferSize() const {
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

} // namespace Innsmouth