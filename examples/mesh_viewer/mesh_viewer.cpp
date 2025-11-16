#include "innsmouth/application/application.h"
#include "innsmouth/graphics/pipeline/graphics_pipeline.h"

using namespace Innsmouth;

class MeshViewer : public Innsmouth::Layer {
public:
  void OnSwapchain() override {
  }

  void OnImGui() override {
    ImGui::Begin("Window1");
    ImGui::End();

    ImGui::Begin("Window2");
    ImGui::End();
  }

  void OnUpdate(CommandBuffer &command_buffer) override {
    auto &swapchain = Application::Get().GetSwapchain();

    std::array<VkRenderingAttachmentInfo, 1> rendering_ai = {};
    {
      rendering_ai[0].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
      rendering_ai[0].imageView = swapchain.GetCurrentImageView();
      rendering_ai[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      rendering_ai[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
      rendering_ai[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      rendering_ai[0].clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};
    }

    auto extent = swapchain.GetExtent();

    command_buffer.CommandBeginRendering(swapchain.GetExtent(), rendering_ai);
    command_buffer.CommandSetViewport(0.0f, 0.0f, extent.width, extent.height);
    command_buffer.CommandSetScissor(0, 0, extent.width, extent.height);
    command_buffer.CommandEndRendering();
  }

  void OnAttach() override {

  }

private:

};

int main() {

  Application application;

  MeshViewer mesh_viewer;

  application.AddLayer(&mesh_viewer);

  application.Run();

  return 0;
}