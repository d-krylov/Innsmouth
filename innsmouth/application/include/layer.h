#ifndef INNSMOUTH_LAYER_H
#define INNSMOUTH_LAYER_H

#include "gui/include/event.h"

namespace Innsmouth {

class CommandBuffer;

class Layer {
public:
  virtual ~Layer() = default;

  virtual void OnSwapchain() {}
  virtual void OnImGui() {}
  virtual void OnUpdate(CommandBuffer &command_buffer) {}
  virtual void OnAttach() {}
  virtual void OnEvent(Event &event) {}
};

} // namespace Innsmouth

#endif // INNSMOUTH_LAYER_H