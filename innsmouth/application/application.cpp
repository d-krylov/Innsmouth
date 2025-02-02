#include "application/include/application.h"
#include "graphics/include/structure_tools.h"
#include <ranges>
#include <GLFW/glfw3.h>

namespace Innsmouth {

std::vector<const char *> GetExtensions() {
  uint32_t extensions_count = 0;
  auto extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
  return std::vector(extensions, extensions + extensions_count);
}

Application::Application(std::string_view name, uint32_t width, uint32_t height)
  : window_(name, width, height),
    graphics_(GetExtensions(), DebugOptions(DebugMessageType::GENERAL | DebugMessageType::VALIDATION,
                                            DebugMessageSeverity::ERROR | DebugMessageSeverity::VERBOSE | DebugMessageSeverity::INFO)),
    swapchain_(window_) {
  Initialize();
}

void Application::Initialize() {
  for (auto &image_view : swapchain_) {
    fences_.emplace_back(true);
    command_buffers_.emplace_back(GraphicsCommandPool());
    image_available_semaphores.emplace_back();
    render_finished_semaphores.emplace_back();
  }
}

void Application::Run() {
  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  while (window_.ShouldClose() == false) {
    window_.PollEvents();

    const auto &image_available = image_available_semaphores[current_frame_];
    const auto &render_finished = render_finished_semaphores[current_frame_];

    fences_[current_frame_].Wait();

    auto result = swapchain_.AcquireNextImage(image_available);

    if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR)) {
      if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        swapchain_.Recreate();
      }
      continue;
    } else {
      VK_CHECK(result);
    }

    auto image_index = swapchain_.GetCurrentImageIndex();

    auto &command_buffer = command_buffers_[current_frame_];

    fences_[current_frame_].Reset();

    command_buffer.Reset();
    command_buffer.Begin();

    VkRenderingAttachmentInfo rendering_attachment_info{};

    rendering_attachment_info = CreateRenderingAttachmentInfo(swapchain_.GetCurrentImageView(), LoadOperation::CLEAR, StoreOperation::STORE,
                                                              ImageLayout::COLOR_ATTACHMENT_OPTIMAL);

    command_buffer.CommandBeginRendering(swapchain_.GetSurfaceExtent(), std::views::single(rendering_attachment_info));

    command_buffer.CommandEndRendering();

    command_buffer.End();

    VkSubmitInfo submit_info{};
    {
      submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submit_info.waitSemaphoreCount = 1;
      submit_info.pWaitSemaphores = image_available.get();
      submit_info.pWaitDstStageMask = wait_stages;
      submit_info.commandBufferCount = 1;
      submit_info.pCommandBuffers = command_buffer.get();
      submit_info.signalSemaphoreCount = 1;
      submit_info.pSignalSemaphores = render_finished.get();
    }

    VK_CHECK(vkQueueSubmit(GraphicsQueue(), 1, &submit_info, fences_[current_frame_]));

    result = swapchain_.Present(render_finished.get());

    current_frame_ = (current_frame_ + 1) % (swapchain_.GetImageCount() - 1);
  }
}

} // namespace Innsmouth