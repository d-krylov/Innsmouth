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

    Innsmouth::CoreImage core_image("../assets/images/container.png");

    image_ = std::make_unique<Innsmouth::Image2D>(core_image);
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

    auto write_descriptor =
      image_->GetWriteDescriptorSet(0, Innsmouth::DescriptorType::COMBINED_IMAGE_SAMPLER);

    command_buffer.CommandPushDescriptorSet(
      *graphics_pipeline_, 0, Innsmouth::ToSpan(write_descriptor.write_descriptor_set_));

    command_buffer.CommandPushConstants(*graphics_pipeline_, Innsmouth::ShaderStage::VERTEX,
                                        std::as_bytes(Innsmouth::ToSpan(pc)));

    command_buffer.CommandDraw(36, 1, 0, 0);
  }

private:
  std::unique_ptr<Innsmouth::GraphicsPipeline> graphics_pipeline_;
  std::unique_ptr<Innsmouth::Image2D> image_;
  Innsmouth::Buffer buffer_;
  Innsmouth::Camera camera_;
};

int main() {

  MyApp app;

  app.Run();

  return 0;
}