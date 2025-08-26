#include "innsmouth/common/innsmouth.h"

using namespace Innsmouth;

class Triangle : public Innsmouth::Layer {
public:
  void OnSwapchain() override {
  }

  void OnImGui() override {
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
    command_buffer.CommandEnableDepthTest(false);
    command_buffer.CommandSetCullMode(VK_CULL_MODE_NONE);
    command_buffer.CommandEnableDepthWrite(false);
    command_buffer.CommandBindVertexBuffer(vertex_buffer_->GetHandle(), 0);
    command_buffer.CommandBindGraphicsPipeline(graphics_pipeline_->GetPipeline());
    command_buffer.CommandDraw(3);
    command_buffer.CommandEndRendering();
  }

  void OnAttach() override {
    // clang-format off
    std::array triangle{
      -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
      +0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
      +0.0f, +0.5f, 0.0f, 0.0f, 1.0f,
    };
    // clang-format on

    auto format = Application::Get().GetSwapchain().GetFormat();

    vertex_buffer_ = std::make_unique<Buffer>(10_KiB, VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    vertex_buffer_->Map();
    auto mapped_data = vertex_buffer_->GetMappedData<float>();
    std::copy(triangle.begin(), triangle.end(), mapped_data.begin());
    vertex_buffer_->Unmap();

    graphics_pipeline_ = std::make_unique<GraphicsPipeline>(GetInnsmouthShadersDirectory() / "tests" / "2d.vert.spv",
                                                            GetInnsmouthShadersDirectory() / "tests" / "2d.frag.spv", format);
  }

private:
  std::unique_ptr<GraphicsPipeline> graphics_pipeline_;
  std::unique_ptr<Buffer> vertex_buffer_;
};

int main() {

  Application application;

  Triangle triangle;

  application.AddLayer(&triangle);

  application.Run();

  return 0;
}