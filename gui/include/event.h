#ifndef INNSMOUTH_EVENT_H
#define INNSMOUTH_EVENT_H

#include <cstdint>

namespace Innsmouth {

enum class EventType {
  NONE,
  WINDOW_CLOSE,
  WINDOW_RESIZE,
  WINDOW_FOCUS,
  WINDOW_MOVE,
  KEY_PRESS,
  KEY_RELEASE,
  KEY_REPEAT,
  MOUSE_BUTTON_PRESS,
  MOUSE_BUTTON_RELEASE,
  MOUSE_MOVE,
  MOUSE_SCROLL
};

class Event {
public:
  virtual ~Event() = default;

  virtual EventType GetEventType() const = 0;

private:
  bool handled_{false};
};

class KeyPressEvent : public Event {
public:
  KeyPressEvent(int32_t key, int32_t scancode, int32_t modifiers);

  int32_t GetKey() const { return key_; }

  [[nodiscard]] static EventType GetStaticType() { return EventType::KEY_PRESS; }
  [[nodiscard]] EventType GetEventType() const override { return GetStaticType(); }

private:
  int32_t key_;
  int32_t scancode_;
  int32_t modifiers_;
};

class KeyReleaseEvent : public Event {
public:
  KeyReleaseEvent(int32_t key, int32_t scancode, int32_t modifiers);

  int32_t GetKey() const { return key_; }

  [[nodiscard]] static EventType GetStaticType() { return EventType::KEY_RELEASE; }
  [[nodiscard]] EventType GetEventType() const override { return GetStaticType(); }

private:
  int32_t key_;
  int32_t scancode_;
  int32_t modifiers_;
};

class KeyRepeatEvent : public Event {
public:
  KeyRepeatEvent(int32_t key, int32_t scancode, int32_t modifiers);

  int32_t GetKey() const { return key_; }

  [[nodiscard]] static EventType GetStaticType() { return EventType::KEY_REPEAT; }
  [[nodiscard]] EventType GetEventType() const override { return GetStaticType(); }

private:
  int32_t key_;
  int32_t scancode_;
  int32_t modifiers_;
};

class WindowResizeEvent : public Event {
public:
  WindowResizeEvent(int32_t width, int32_t height) : width_(width), height_(height) {}

  EventType GetEventType() const override { return EventType::WINDOW_RESIZE; }

private:
  int32_t width_;
  int32_t height_;
};

class MouseButtonPressEvent : public Event {
public:
  MouseButtonPressEvent(int32_t button, int32_t modifiers);

  [[nodiscard]] static EventType GetStaticType() { return EventType::MOUSE_BUTTON_PRESS; }
  [[nodiscard]] EventType GetEventType() const override { return GetStaticType(); }

private:
  int32_t button_;
  int32_t modifiers_;
};

class MouseButtonReleaseEvent : public Event {
public:
  MouseButtonReleaseEvent(int32_t button, int32_t modifiers);

  [[nodiscard]] static EventType GetStaticType() { return EventType::MOUSE_BUTTON_RELEASE; }
  [[nodiscard]] EventType GetEventType() const override { return GetStaticType(); }

private:
  int32_t button_;
  int32_t modifiers_;
};

class MouseMoveEvent : public Event {
public:
  MouseMoveEvent(double x, double y) : x_(x), y_(y) {}

  EventType GetEventType() const override { return EventType::MOUSE_MOVE; }

private:
  double x_;
  double y_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_EVENT_H