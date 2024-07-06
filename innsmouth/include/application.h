#ifndef INNSMOUTH_APPLICATION_H
#define INNSMOUTH_APPLICATION_H

#include "gui/imgui/imgui_platform.h"
#include "gui/imgui/imgui_renderer.h"

namespace Innsmouth {

class Application {
public:
  Application(std::string_view name, uint32_t width, uint32_t height);

  virtual ~Application();

  void Run();

  virtual void OnImGui() {}

  virtual void OnUpdate(CommandBuffer &command_buffer) {}

protected:
  void Initialize();

protected:
  Window window_;
  ImGuiPlatform imgui_platform_;
  std::unique_ptr<Swapchain> swapchain_;
  std::unique_ptr<ImGuiRenderer> imgui_renderer_;
  std::vector<CommandBuffer> command_buffers_;
  std::vector<Semaphore> image_available_semaphores;
  std::vector<Semaphore> render_finished_semaphores;
  std::vector<Fence> fences_;
  uint32_t current_frame_{0};
};

} // namespace Innsmouth

#endif // INNSMOUTH_APPLICATION_H