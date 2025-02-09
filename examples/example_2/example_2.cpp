#include "application/include/innsmouth.h"
#include "imgui.h"
#include <ranges>

using namespace Innsmouth;

class Example : public Layer {
public:
  Example() {}

  void OnSwapchain() override {}
  void OnImGui() override {
    ImGui::Begin("hello");
    ImGui::End();
  }

  void OnUpdate(CommandBuffer &command_buffer) override {
    auto &swapchain = Application::Get().GetSwapchain();
    std::array attachments{CreateRenderingAttachmentInfo(swapchain.GetCurrentImageView())};
    command_buffer.CommandBeginRendering(swapchain.GetSurfaceExtent(), attachments);
    command_buffer.CommandEndRendering();
  }

  void OnAttach() override {}

  void OnEvent(Event &event) override {}

private:
};

int main() {
  Application application("Example", 800, 600);

  Example example;

  application.AddLayer(&example);

  application.Run();

  return 0;
}