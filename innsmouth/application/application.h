#ifndef INNSMOUTH_APPLICATION_H
#define INNSMOUTH_APPLICATION_H

#include "innsmouth/gui/window/window.h"
#include "innsmouth/graphics/presentation/swapchain.h"
#include "innsmouth/graphics/graphics_context/graphics_context.h"
#include "innsmouth/graphics/graphics_context/graphics_allocator.h"
#include "innsmouth/graphics/synchronization/fence.h"
#include "innsmouth/graphics/synchronization/semaphore.h"
#include "innsmouth/graphics/command/command_buffer.h"
#include "innsmouth/graphics/command/command_pool.h"
#include "innsmouth/gui/imgui/imgui_layer.h"
#include "innsmouth/gui/imgui/imgui_renderer.h"
#include "layer.h"

namespace Innsmouth {

class Application {

public:
  Application();

  void Run();

  void AddLayer(Layer *layer);

  static Application *Get();

  const Swapchain &GetSwapchain() const;

  void OnSwapchain();
  void OnEvent(Event &event);

protected:
  void Initialize();

private:
  Window main_window_;
  GraphicsContext graphics_context_;
  GraphicsAllocator graphics_allocator_;
  CommandPool command_pool_;
  Swapchain swapchain_;
  ImGuiLayer imgui_layer_;
  ImGuiRenderer imgui_renderer_;
  std::vector<Fence> fences_;
  std::vector<Semaphore> image_available_semaphores;
  std::vector<Semaphore> render_finished_semaphores;
  std::vector<CommandBuffer> command_buffers_;
  std::vector<Layer *> layers_;

  uint32_t current_frame_{0};

  static Application *application_instance_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_APPLICATION_H