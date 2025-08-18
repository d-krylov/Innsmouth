#ifndef INNSMOUTH_WINDOW_H
#define INNSMOUTH_WINDOW_H

#include <cstdint>
#include <functional>
#include <string_view>

struct GLFWwindow;

namespace Innsmouth {

class Window {
public:
  struct Extent2i {
    int32_t width;
    int32_t height;
  };

  Window(std::string_view name, int32_t width, int32_t height);

  GLFWwindow *GetNativeWindow() const {
    return native_window_;
  }

  Extent2i GetFramebufferSize() const;
  Extent2i GetSize() const;
  int32_t GetWidth() const;
  int32_t GetHeight() const;

  float GetAspectRatio() const;

  bool ShouldClose() const;

  void PollEvents();

protected:
  void SetCallbacks();

private:
  GLFWwindow *native_window_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_WINDOW_H