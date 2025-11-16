#include "innsmouth/common/innsmouth.h"

using namespace Innsmouth;

std::vector<int32_t> MakeTexture(uint32_t width, uint32_t height) {
  std::vector<int32_t> ret(width * height);
  for (auto x = 0; x < width; x++) {
    for (auto y = 0; y < height; y++) {
      ret[y * width + x] = ((x & 255) << 16) | ((y & 255) << 8);
    }
  }
  return ret;
}

class Texture : public Innsmouth::Layer {
public:
  void OnSwapchain() override {
  }

  void OnImGui() override {
    ImGui::Begin("hello");
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
    command_buffer.CommandEnableDepthTest(false);
    command_buffer.CommandSetCullMode(VK_CULL_MODE_NONE);
    command_buffer.CommandEnableDepthWrite(false);
    command_buffer.CommandBindGraphicsPipeline(graphics_pipeline_->GetPipeline());
    command_buffer.CommandPushDescriptorSet(graphics_pipeline_->GetPipelineLayout(), 0, 0, image_->GetImageView(), image_->GetSampler());
    command_buffer.CommandDraw(3);
    command_buffer.CommandEndRendering();
  }

  void OnAttach() override {
    auto format = Application::Get().GetSwapchain().GetFormat();

    auto image_data = MakeTexture(800, 600);

    image_ = std::make_unique<Image>(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM,
                                     ImageSpecification(800, 600));

    image_->SetData<int32_t>(image_data);

    graphics_pipeline_ = std::make_unique<GraphicsPipeline>(GetInnsmouthShadersDirectory() / "tests" / "square.vert.spv",
                                                            GetInnsmouthShadersDirectory() / "tests" / "square.frag.spv", format);
  }

private:
  std::unique_ptr<GraphicsPipeline> graphics_pipeline_;
  std::unique_ptr<Image> image_;
};

int main() {

  Application application;

  Texture texture;

  application.AddLayer(&texture);

  application.Run();

  return 0;
}