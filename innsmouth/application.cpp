#include "innsmouth/include/application.h"

namespace Innsmouth {

Application::Application(std::string_view name, uint32_t width, uint32_t height)
  : window_(name, Vector2i(width, height)), imgui_platform_(&window_) {
  CreateGraphics(GraphicsDescription::CreateDefault());
  Initialize();
}

Application::~Application() {}

void Application::Initialize() {
  swapchain_ = std::make_unique<Swapchain>(window_);

  imgui_renderer_ = std::make_unique<ImGuiRenderer>(*swapchain_);

  const auto &image_views = swapchain_->GetImageViews();

  for (std::size_t i = 0; i < image_views.size(); i++) {
    fences_.emplace_back(true);
    command_buffers_.emplace_back(CommandPool());
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

    auto result = swapchain_->AcquireNextImage(image_available);

    if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR)) {

      if (result == VK_ERROR_OUT_OF_DATE_KHR) {

        swapchain_->Recreate();
      }

      continue;

    } else {

      VK_CHECK(result);
    }

    auto image_index = swapchain_->GetCurrentImageIndex();

    auto &command_buffer = command_buffers_[current_frame_];

    fences_[current_frame_].Reset();

    command_buffer.Reset();
    command_buffer.Begin();

    command_buffer.CommandBeginRendering(swapchain_->GetImageViews()[image_index],
                                         swapchain_->GetSurfaceCapabilities().currentExtent);

    OnUpdate(command_buffer);

    imgui_platform_.NewFrame();
    imgui_renderer_->Begin();
    OnImGui();
    imgui_renderer_->End(command_buffer);

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

    result = swapchain_->Present(render_finished);

    current_frame_ = (current_frame_ + 1) % (swapchain_->GetImageCount() - 1);
  }
}

} // namespace Innsmouth