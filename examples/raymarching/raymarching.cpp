#include "core/include/tools.h"
#include "easyloggingpp/easylogging++.h"
#include "innsmouth/include/application.h"
#include "mesh/include/primitives.h"
#include "scene/include/camera.h"

INITIALIZE_EASYLOGGINGPP

struct PushConstants {
  Innsmouth::Matrix4f projection_matrix_;
  Innsmouth::Matrix4f view_matrix_;
  Innsmouth::Matrix4f model_matrix_;
};

class MyApp : public Innsmouth::Application {
public:
  MyApp()
    : Application("MyApp", 800, 600),
      buffer_(Innsmouth::BufferUsage::VERTEX_BUFFER, 1024 * 1024 * 10) {
    Innsmouth::GraphicsPipelineDescription description{
      .paths_ = std::vector<std::filesystem::path>{"./main.vert.spv", "./main.frag.spv"},
      .color_formats_ = {swapchain_->GetSurfaceFormat().format}};

    graphics_pipeline_ = std::make_unique<Innsmouth::GraphicsPipeline>(description);

    auto box = Innsmouth::MakeBox();

    buffer_.SetData(std::span<Innsmouth::Vertex>(box));

    camera_.SetPosition(Innsmouth::Vector3f(0.0f, 0.0f, 4.0f));
  }

  void OnUpdate(Innsmouth::CommandBuffer &command_buffer) override {

    auto extent = swapchain_->GetSurfaceCapabilities().currentExtent;

    command_buffer.CommandSetViewport((float)extent.width, (float)extent.height);
    command_buffer.CommandSetScissor(extent.width, extent.height);
    command_buffer.CommandBindPipeline(*graphics_pipeline_);
    command_buffer.CommandBindVertexBuffer(buffer_);

    PushConstants pc{.projection_matrix_ = camera_.GetPerspectiveMatrix(),
                     .view_matrix_ = camera_.GetLookAtMatrix(),
                     .model_matrix_ = glm::rotate(Innsmouth::Matrix4f(1.0f),
                                                  float(Innsmouth::GetTime()), Innsmouth::Y_)};

    command_buffer.CommandPushConstants(*graphics_pipeline_,
                                        VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT,
                                        std::as_bytes(Innsmouth::ToSpan(pc)));

    command_buffer.CommandDraw(36, 1, 0, 0);
  }

private:
  std::unique_ptr<Innsmouth::GraphicsPipeline> graphics_pipeline_;
  Innsmouth::Buffer buffer_;
  Innsmouth::Camera camera_;
};

int main() {

  MyApp app;

  app.Run();

  return 0;
}