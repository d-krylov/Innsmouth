#ifndef INNSMOUTH_APPLICATION_H
#define INNSMOUTH_APPLICATION_H

#include "gui/include/window.h"
#include "graphics/include/graphics.h"
#include "graphics/include/swapchain.h"
#include "graphics/synchronization/semaphore.h"
#include "graphics/synchronization/fence.h"
#include "graphics/buffer/command_buffer.h"
#include "gui/imgui/imgui_layer.h"
#include "gui/imgui/imgui_renderer.h"

namespace Innsmouth {

class Layer;

class Application {
public:
  Application(std::string_view name, uint32_t width, uint32_t height);

  void AddLayer(Layer *layer);

  void Run();

  void OnEvent(Event &event);

  static Application &Get();

  const Swapchain &GetSwapchain() const { return swapchain_; }

protected:
  void Initialize();

private:
  Window window_;
  Graphics graphics_;
  Swapchain swapchain_;
  ImGuiLayer imgui_layer_;
  ImGuiRenderer imgui_renderer_;
  std::vector<CommandBuffer> command_buffers_;
  std::vector<Semaphore> image_available_semaphores;
  std::vector<Semaphore> render_finished_semaphores;
  std::vector<Fence> fences_;
  uint32_t current_frame_{0};
  std::vector<Layer *> layers_;

  static Application *application_instance_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_APPLICATION_H