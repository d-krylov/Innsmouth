#ifndef INNSMOUTH_WINDOW_H
#define INNSMOUTH_WINDOW_H

#include "innsmouth/core/include/core.h"
#include <functional>
#include <string_view>

struct GLFWwindow;

namespace Innsmouth {

class Event;

class Window {
public:
  using EventHandler = std::function<void(Event &)>;

  Window(std::string_view name, int32_t width, int32_t height);

  GLFWwindow *GetNativeWindow() const {
    return native_window_;
  }

  ViewportSize GetFramebufferSize() const;
  ViewportSize GetSize() const;
  int32_t GetWidth() const;
  int32_t GetHeight() const;

  int32_t GetKey(int32_t key) const;

  float GetAspect() const;

  bool ShouldClose() const;

  void PollEvents();

  void Invoke(Event &event);

  void SetEventHandler(const EventHandler &handler);

private:
  GLFWwindow *native_window_;
  EventHandler event_handler_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_WINDOW_H