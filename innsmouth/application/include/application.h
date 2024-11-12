#ifndef INNSMOUTH_APPLICATION_H
#define INNSMOUTH_APPLICATION_H

#include "graphics/include/graphics.h"
#include "gui/imgui/imgui_layer.h"
#include "gui/imgui/imgui_renderer.h"
#include "gui/include/window.h"

namespace Innsmouth {

class Application {
public:
  Application(std::string_view name, uint32_t width, uint32_t height);

  virtual ~Application();

  void Run();

  void OnEvent(Event &event);

  [[nodiscard]] static Application &Get() { return *application_instance_; }

  [[nodiscard]] Swapchain &GetSwapchain() { return swapchain_; }

  void AddLayer(Layer *layer);

protected:
  void Initialize();

private:
  Graphics graphics_;
  Window window_;
  Swapchain swapchain_;
  ImGuiLayer imgui_layer_;
  ImGuiRenderer imgui_renderer_;
  std::vector<CommandBuffer> command_buffers_;
  std::vector<Semaphore> image_available_semaphores;
  std::vector<Semaphore> render_finished_semaphores;
  std::vector<Fence> fences_;
  std::vector<Layer *> layers_;
  uint32_t current_frame_{0};

  static Application *application_instance_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_APPLICATION_H