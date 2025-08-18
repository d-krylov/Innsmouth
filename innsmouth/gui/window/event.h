#ifndef INNSMOUTH_EVENT_H
#define INNSMOUTH_EVENT_H

#include "key.h"
#include <string_view>

namespace Innsmouth {

#define EVENT_CLASS_KIND(kind)                                                                                                                  \
  static EventKind GetStaticKind() { return EventKind::kind; }                                                                                  \
  EventKind GetEventKind() const override { return GetStaticKind(); }                                                                           \
  std::string_view GetName() const override { return #kind; }

class Event {
public:
  virtual ~Event() = default;

  virtual EventKind GetEventKind() const = 0;
  virtual std::string_view GetName() const = 0;

public:
  bool handled_{false};
};

} // namespace Innsmouth

#endif // INNSMOUTH_EVENT_H