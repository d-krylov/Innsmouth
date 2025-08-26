#ifndef INNSMOUTH_MOUSE_EVENTS_H
#define INNSMOUTH_MOUSE_EVENTS_H

#include "event.h"

namespace Innsmouth {

class MouseButtonEvent : public Event {
public:
  static constexpr auto event_kind = EventKind::MOUSE_BUTTON;

  MouseButtonEvent(MouseButton button, Action action) : button_(button), action_(action) {
  }

  MouseButton GetMouseButton() const {
    return button_;
  }

  Action GetAction() const {
    return action_;
  }

  EventKind GetEventKind() override {
    return event_kind;
  }

private:
  MouseButton button_;
  Action action_;
};

class MouseScrollEvent : public Event {
public:
  static constexpr auto event_kind = EventKind::MOUSE_SCROLL;

  MouseScrollEvent(float x, float y) : x_(x), y_(y) {
  }

  float GetX() const {
    return x_;
  }

  float GetY() const {
    return y_;
  }

  EventKind GetEventKind() override {
    return event_kind;
  }

private:
  float x_;
  float y_;
};

class MouseMoveEvent : public Event {
public:
  static constexpr auto event_kind = EventKind::MOUSE_MOVE;

  MouseMoveEvent(float x, float y) : x_(x), y_(y) {
  }

  float GetX() const {
    return x_;
  }

  float GetY() const {
    return y_;
  }

  EventKind GetEventKind() override {
    return event_kind;
  }

private:
  float x_;
  float y_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_MOUSE_EVENTS_H