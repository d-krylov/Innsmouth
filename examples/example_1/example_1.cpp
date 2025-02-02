#include "application/include/innsmouth.h"
#include <ranges>

using namespace Innsmouth;

class Example : public Layer {
public:
  void OnSwapchain() override {}
  void OnImGui() override {}
  void OnUpdate(CommandBuffer &command_buffer) override {

    auto &swapchain = Application::Get().GetSwapchain();

    auto rendering_attachment_info = CreateRenderingAttachmentInfo(swapchain.GetCurrentImageView(), LoadOperation::CLEAR,
                                                                   StoreOperation::STORE, ImageLayout::COLOR_ATTACHMENT_OPTIMAL);

    auto extent = swapchain.GetSurfaceExtent();

    command_buffer.CommandBeginRendering(extent, std::views::single(rendering_attachment_info));

    command_buffer.CommandBindPipeline(*graphics_pipeline_);

    command_buffer.CommandSetViewport(0.0f, 0.0f, extent.width, extent.height);

    command_buffer.CommandSetScissor(0, 0, extent.width, extent.height);

    command_buffer.CommandEnableDepthTest(false);

    command_buffer.CommandEnableDepthWrite(false);

    command_buffer.CommandSetCullMode(CullMode::NONE);

    command_buffer.CommandDraw(3);

    command_buffer.CommandEndRendering();
  }

  void OnAttach() override {
    auto &swapchain = Application::Get().GetSwapchain();

    graphics_pipeline_ = std::make_unique<GraphicsPipeline>("../shaders/spirv/canvas.vert.spv", "../shaders/spirv/canvas.frag.spv",
                                                            swapchain.GetSurfaceFormat());
  }

  void OnEvent(Event &event) override {}

private:
  std::unique_ptr<GraphicsPipeline> graphics_pipeline_;
};

int main() {

  Application application("Example", 800, 600);

  Example example;

  application.AddLayer(&example);

  application.Run();

  return 0;
}