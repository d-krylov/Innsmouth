#include "application.h"

namespace Innsmouth {

Application *Application::application_instance_ = nullptr;

Application *Application::Get() {
  return application_instance_;
}

Application::Application()
  : main_window_("Innsmouth", 800, 600), swapchain_(main_window_.GetNativeWindow()), imgui_layer_(&main_window_),
    imgui_renderer_(swapchain_.GetFormat()) {
  Initialize();

  main_window_.SetEventHandler(BIND_FUNCTION(Application::OnEvent));
  layers_.push_back(&imgui_layer_);

  application_instance_ = this;
}

void Application::Initialize() {
  for (const auto &image_view : swapchain_.GetImageViews()) {
    fences_.emplace_back(FenceCreateMaskBits::E_SIGNALED_BIT);
    command_buffers_.emplace_back(GraphicsContext::Get()->GetGeneralCommandPool());
    image_available_semaphores.emplace_back();
    render_finished_semaphores.emplace_back();
  }
}

void Application::AddLayer(Layer *layer) {
  auto &new_layer = layers_.emplace_back(layer);
  new_layer->OnAttach();
}

void Application::OnEvent(Event &event) {
  for (auto &layer : layers_) {
    layer->OnEvent(event);
  }
}

void Application::Run() {

  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  while (main_window_.ShouldClose() == false) {
    main_window_.PollEvents();

    fences_[current_frame_].Wait();

    auto result = swapchain_.AcquireNextImage(image_available_semaphores[current_frame_]);

    if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR)) {
      if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        swapchain_.Recreate();
      }
      continue;
    } else {
      VK_CHECK(result);
    }

    fences_[current_frame_].Reset();

    command_buffers_[current_frame_].Reset();
    command_buffers_[current_frame_].Begin();

    for (auto &layer : layers_) {
      layer->OnUpdate(command_buffers_[current_frame_]);
    }

    imgui_layer_.NewFrame();
    imgui_renderer_.Begin(command_buffers_[current_frame_], swapchain_);

    for (auto layer : layers_) {
      layer->OnImGui();
    }

    imgui_renderer_.End(command_buffers_[current_frame_]);

    command_buffers_[current_frame_].End();

    auto index = swapchain_.GetCurrentImageIndex();
    auto &render_finished_semaphore = render_finished_semaphores[index];

    VkSubmitInfo submit_info{};
    {
      submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submit_info.waitSemaphoreCount = 1;
      submit_info.pWaitSemaphores = image_available_semaphores[current_frame_].get();
      submit_info.pWaitDstStageMask = wait_stages;
      submit_info.commandBufferCount = 1;
      submit_info.pCommandBuffers = command_buffers_[current_frame_].get();
      submit_info.signalSemaphoreCount = 1;
      submit_info.pSignalSemaphores = render_finished_semaphore.get();
    }

    VK_CHECK(vkQueueSubmit(GraphicsContext::Get()->GetGeneralQueue(), 1, &submit_info, fences_[current_frame_].GetHandle()));

    result = swapchain_.Present(render_finished_semaphore.get());

    current_frame_ = (current_frame_ + 1) % (swapchain_.GetImageCount() - 1);
  }
}

} // namespace Innsmouth