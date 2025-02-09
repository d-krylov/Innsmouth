#include "application/include/innsmouth.h"
#include "imgui.h"
#include <ranges>
#include <iostream>

using namespace Innsmouth;

struct Push {
  Matrix4f p = glm::identity<Matrix4f>();
  Matrix4f v = glm::identity<Matrix4f>();
  Matrix4f m = glm::identity<Matrix4f>();
};

class Example : public Layer {
public:
  Example() : buffer_(100_MiB, BufferUsage::VERTEX_BUFFER_BIT), ubo_(10_KiB, BufferUsage::UNIFORM_BUFFER_BIT) {}

  void OnSwapchain() override {}
  void OnImGui() override {}

  void OnUpdate(CommandBuffer &command_buffer) override {

    Push push;

    push.v = camera_.GetLookAtMatrix();
    push.p = camera_.GetProjectionMatrix();

    auto &swapchain = Application::Get().GetSwapchain();

    auto rendering_attachment_info = CreateRenderingAttachmentInfo(swapchain.GetCurrentImageView(), LoadOperation::CLEAR,
                                                                   StoreOperation::STORE, ImageLayout::COLOR_ATTACHMENT_OPTIMAL);

    auto extent = swapchain.GetSurfaceExtent();

    command_buffer.CommandBeginRendering(extent, std::views::single(rendering_attachment_info));
    command_buffer.CommandBindPipeline(*graphics_pipeline_);
    command_buffer.CommandPushConstants(graphics_pipeline_->GetPipelineLayout(), ShaderStage::VERTEX_BIT, push);

    command_buffer.CommandPushDescriptorSet(graphics_pipeline_->GetPipelineLayout(), 0, 0, ubo_, 0, 1024);

    command_buffer.CommandBindVertexBuffer(buffer_);

    command_buffer.CommandSetViewport(0.0f, 0.0f, extent.width, extent.height);

    command_buffer.CommandSetScissor(0, 0, extent.width, extent.height);

    command_buffer.CommandEnableDepthTest(true);

    command_buffer.CommandEnableDepthWrite(true);

    command_buffer.CommandSetCullMode(CullMode::NONE);

    for (auto &mesh : model_.meshes_) {
      auto ambient = mesh.material_.texture_names_[std::to_underlying(TextureType::DIFFUSE)];

      auto &image = model_.textures_.at(ambient);
      command_buffer.CommandPushDescriptorSet(graphics_pipeline_->GetPipelineLayout(), 0, 1, image);

      command_buffer.CommandDraw(mesh.size_, 1, mesh.offset_);
    }

    command_buffer.CommandEndRendering();
  }

  void OnAttach() override {
    auto &swapchain = Application::Get().GetSwapchain();

    std::filesystem::path mesh_path = "../../Models/sponza2/sponza.obj";

    model_ = LoadWavefrontModel(mesh_path);

    buffer_.Map();
    buffer_.Memcpy<TexturedVertex>(model_.vertices_);
    buffer_.Unmap();

    camera_.SetPosition(Vector3f(0.0f, 5.0f, 0.0f));

    std::vector<Vector3f> camera{camera_.GetPosition()};

    ubo_.Map();
    ubo_.Memcpy<Vector3f>(camera);
    ubo_.Unmap();

    graphics_pipeline_ = std::make_unique<GraphicsPipeline>("../shaders/spirv/mesh/textured_mesh.vert.spv",
                                                            "../shaders/spirv/mesh/textured_mesh.frag.spv", swapchain.GetSurfaceFormat());
  }

  void OnEvent(Event &event) override {}

private:
  Buffer buffer_;
  Buffer ubo_;
  Camera camera_;
  Model model_;
  std::unique_ptr<GraphicsPipeline> graphics_pipeline_;
};

int main() {

  Application application("Example", 800, 600);

  Example example;

  application.AddLayer(&example);

  application.Run();

  return 0;
}