#ifndef INNSMOUTH_GRAPHICS_DESCRIPTION_H
#define INNSMOUTH_GRAPHICS_DESCRIPTION_H

#include "innsmouth/graphics/include/graphics_types.h"
#include <string>
#include <vector>

namespace Innsmouth {

struct GraphicsDescription {

  [[nodiscard]] static GraphicsDescription CreateDefault();

  std::optional<DebugMessageType> message_type_;
  DebugMessageSeverity message_severity_;
  std::string application_name_;
  std::string engine_name_;
  std::vector<const char *> instance_extensions_;
  PFN_vkDebugUtilsMessengerCallbackEXT debug_function_;
  VkPhysicalDeviceFeatures device_features_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_DESCRIPTION_H