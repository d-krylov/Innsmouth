#ifndef INNSMOUTH_APPLICATION_H
#define INNSMOUTH_APPLICATION_H

#include "gui/include/window.h"
#include "graphics/include/graphics.h"
#include "graphics/include/swapchain.h"
#include "graphics/synchronization/semaphore.h"
#include "graphics/synchronization/fence.h"
#include "graphics/buffer/command_buffer.h"

namespace Innsmouth {

class Application {
public:
  Application(std::string_view name, uint32_t width, uint32_t height);

  void Run();

protected:
  void Initialize();

private:
  Window window_;
  Graphics graphics_;
  Swapchain swapchain_;
  std::vector<CommandBuffer> command_buffers_;
  std::vector<Semaphore> image_available_semaphores;
  std::vector<Semaphore> render_finished_semaphores;
  std::vector<Fence> fences_;
  uint32_t current_frame_{0};
};

} // namespace Innsmouth

#endif // INNSMOUTH_APPLICATION_H