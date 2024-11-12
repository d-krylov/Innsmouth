#ifndef INNSMOUTH_IMGUI_LAYER_H
#define INNSMOUTH_IMGUI_LAYER_H

#include "application/include/layer.h"

namespace Innsmouth {

class Window;

class ImGuiLayer : public Layer {
public:
  ImGuiLayer(Window *window);

  ~ImGuiLayer() = default;

  void OnEvent(Event &event) override;

  void NewFrame();

protected:
  void UpdateMouseData();

  bool OnKeyEvent(const KeyEvent &event);
  bool OnMouseButtonEvent(const MouseButtonEvent &event);
  bool OnMouseScrollEvent(const MouseScrollEvent &event);
  bool OnMouseMoveEvent(const MouseMoveEvent &event);
  bool OnCharEvent(const CharEvent &event);

private:
  Window *window_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMGUI_LAYER_H
