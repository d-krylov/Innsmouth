#ifndef INNSMOUTH_WINDOW_H
#define INNSMOUTH_WINDOW_H

#include "core/include/core_types.h"
#include "event.h"
#include "graphics/include/graphics_types.h"
#include <cstdint>
#include <functional>
#include <string_view>

struct GLFWwindow;

namespace Innsmouth {

class Instance;

class Window {
public:
  using EventCallback = std::function<void(Event &)>;

  Window(std::string_view name, const Vector2i &size);

  [[nodiscard]] GLFWwindow *GetNativeWindow() const { return native_window_; }

  [[nodiscard]] int32_t GetWidth() const;
  [[nodiscard]] int32_t GetHeight() const;
  [[nodiscard]] Vector2i GetSize() const;
  [[nodiscard]] Vector2i GetPosition() const;
  [[nodiscard]] Vector2f GetCursorPosition() const;
  [[nodiscard]] Vector2i GetFramebufferSize() const;
  [[nodiscard]] bool ShouldClose() const;
  [[nodiscard]] float GetAspectRatio() const;

  void CreateSurface(const VkInstance instance, VkSurfaceKHR *surface);

  void SetEventCallback(const EventCallback &callback) { event_callback_ = callback; }

  [[nodiscard]] const EventCallback &GetEventCallback() const { return event_callback_; }

  void PollEvents();
  void SetCursor(int32_t index);

protected:
  void SetCallbacks();
  void SetUserPointer();

private:
  GLFWwindow *native_window_{nullptr};
  EventCallback event_callback_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_WINDOW_H