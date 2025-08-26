#ifndef INNSMOUTH_KEY_EVENTS_H
#define INNSMOUTH_KEY_EVENTS_H

#include "event.h"

namespace Innsmouth {

class KeyEvent : public Event {
public:
  static constexpr auto event_kind = EventKind::KEY;

  KeyEvent(Key key, int32_t scancode, Action action) : key_(key), scancode_(scancode), action_(action) {
  }

  Key GetKey() const {
    return key_;
  }

  int32_t GetScanCode() const {
    return scancode_;
  }

  Action GetAction() const {
    return action_;
  }

  EventKind GetEventKind() override {
    return event_kind;
  }

private:
  Key key_;
  Action action_;
  int32_t scancode_;
};

class CharEvent : public Event {
public:
  static constexpr auto event_kind = EventKind::CHAR;

  CharEvent(uint32_t character) : character_(character) {
  }

  uint32_t GetCharacter() const {
    return character_;
  }

  EventKind GetEventKind() override {
    return event_kind;
  }

private:
  uint32_t character_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_KEY_EVENTS_H