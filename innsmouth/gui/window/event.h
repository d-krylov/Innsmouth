#ifndef INNSMOUTH_EVENT_H
#define INNSMOUTH_EVENT_H

#include "key.h"
#include <cstdint>
#include <string_view>

namespace Innsmouth {

#define BIND_FUNCTION(function) [this](auto &&...args) -> decltype(auto) { return this->function(std::forward<decltype(args)>(args)...); }

class Event {
public:
  virtual ~Event() = default;

  virtual EventKind GetEventKind() = 0;

public:
  bool handled{false};
};

class EventDispatcher {
public:
  EventDispatcher(Event &event) : event_(event) {
  }

  template <typename T, typename F> bool Dispatch(const F &function) {
    if (event_.GetEventKind() == T::event_kind) {
      event_.handled |= function(static_cast<T &>(event_));
      return true;
    }
    return false;
  }

private:
  Event &event_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_EVENT_H