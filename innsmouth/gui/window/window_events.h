#ifndef INNSMOUTH_WINDOW_EVENTS_H
#define INNSMOUTH_WINDOW_EVENTS_H

#include "event.h"

namespace Innsmouth {

class WindowResizeEvent : public Event {
public:
  static constexpr auto event_kind = EventKind::WINDOW_RESIZE;

  WindowResizeEvent(int32_t width, int32_t height) : width_(width), height_(height) {
  }

  int32_t GetWidth() const {
    return width_;
  }

  int32_t GetHeight() const {
    return height_;
  }

  float GetAspect() const {
    auto w = static_cast<float>(GetWidth());
    auto h = static_cast<float>(GetHeight());
    return w / h;
  }

  EventKind GetEventKind() override {
    return event_kind;
  }

private:
  int32_t width_;
  int32_t height_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_WINDOW_EVENTS_H