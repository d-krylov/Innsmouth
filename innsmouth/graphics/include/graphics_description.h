#ifndef INNSMOUTH_GRAPHICS_DESCRIPTION_H
#define INNSMOUTH_GRAPHICS_DESCRIPTION_H

#include "graphics/include/graphics_types.h"
#include <string>
#include <vector>

namespace Innsmouth {

struct GraphicsDescription {

  [[nodiscard]] static GraphicsDescription CreateDefault();

  std::optional<DebugMessageType> message_type_;
  DebugMessageSeverity message_severity_;
  std::vector<const char *> instance_extensions_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_DESCRIPTION_H