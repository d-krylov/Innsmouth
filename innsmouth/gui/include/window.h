#ifndef INNSMOUTH_WINDOW_H
#define INNSMOUTH_WINDOW_H

#include "core/include/core_types.h"
#include <functional>
#include <string_view>

struct GLFWwindow;

namespace Innsmouth {

class Event;
enum class Action;
enum class Key;

class Window {
public:
  using EventCallbackFunction = std::function<void(Event &)>;

  Window(std::string_view name, uint32_t width, uint32_t height);

  GLFWwindow *GetNativeWindow() const { return native_window_; }

  Extent2i GetFramebufferSize() const;
  Extent2i GetSize() const;
  int32_t GetWidth() const;
  int32_t GetHeight() const;

  float GetAspectRatio() const;

  Action GetKey(Key key) const;

  void SetEventCallback(const EventCallbackFunction &callback) { event_callback_ = callback; }

  const EventCallbackFunction &GetEventCallback() const { return event_callback_; }

  bool ShouldClose() const;

  void PollEvents();

protected:
  void SetCallbacks();

private:
  GLFWwindow *native_window_;
  EventCallbackFunction event_callback_{[](Event &) {}};
};

} // namespace Innsmouth

#endif // INNSMOUTH_WINDOW_H