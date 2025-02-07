#ifndef INNSMOUTH_IMGUI_LAYER_H
#define INNSMOUTH_IMGUI_LAYER_H

#include "application/include/layer.h"

namespace Innsmouth {

class Window;
class Event;
class KeyEvent;
class MouseButtonEvent;
class MouseScrollEvent;
class MouseMoveEvent;
class CharEvent;

class ImGuiLayer : public Layer {
public:
  ImGuiLayer(Window &window);

  void OnEvent(Event &event) override;

  void NewFrame();

private:
  void UpdateMouseData();

  bool OnKeyEvent(const KeyEvent &event);
  bool OnMouseButtonEvent(const MouseButtonEvent &event);
  bool OnMouseScrollEvent(const MouseScrollEvent &event);
  bool OnMouseMoveEvent(const MouseMoveEvent &event);
  bool OnCharEvent(const CharEvent &event);

private:
  const Window *window_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMGUI_LAYER_H