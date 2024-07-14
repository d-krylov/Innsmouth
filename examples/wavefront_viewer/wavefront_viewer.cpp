#include "imgui.h"
#include "innsmouth/include/application.h"
#include "mesh/include/mesh.h"
#include "scene/include/camera.h"

struct Light {
  Innsmouth::Vector3f position_;
  Innsmouth::Vector3f ambient_;
  Innsmouth::Vector3f diffuse_;
};

struct Matrix {
  Innsmouth::Matrix4f projection_;
  Innsmouth::Matrix4f view_;
  Innsmouth::Matrix4f model_;
};

class MyApp : public Innsmouth::Application {
public:
  MyApp()
    : Application("MyApp", 800, 600),
      vbo_(Innsmouth::BufferUsage::VERTEX_BUFFER, 200 * 1024 * 1024) {

    graphics_pipeline_ = std::make_unique<Innsmouth::GraphicsPipeline>(
      std::vector<std::filesystem::path>{"./mesh.vert.spv", "./mesh.frag.spv"},
      std::vector<VkFormat>{swapchain_->GetSurfaceFormat().format}, Innsmouth::Depth::READ_WRITE,
      depth_image_->GetFormat());

    mesh_.Load("../assets/wavefront/sponza_min/sponza.obj");

    std::span<Innsmouth::Vertex> data(mesh_.vertices_);

    vbo_.SetData(std::as_bytes(data));
  }

  void OnImGui() override {
    ImGui::Begin("Settings");
    ImGui::InputFloat3("Camera position", glm::value_ptr(camera_position));
    ImGui::SliderFloat3("Model rotation", glm::value_ptr(rotation_), 0.0f, 6.28f);
    ImGui::End();

    camera_.SetPosition(camera_position);
  }

  void OnUpdate(Innsmouth::CommandBuffer &command_buffer) override {
    auto extent = swapchain_->GetSurfaceCapabilities().currentExtent;

    command_buffer.CommandSetCullMode(false, true);
    command_buffer.CommandSetFrontFace(Innsmouth::FrontFace::COUNTER_CLOCKWISE);
    command_buffer.CommandSetViewport(0.0f, 0.0f, (float)extent.width, (float)extent.height);
    command_buffer.CommandSetScissor({{0, 0}, {extent.width, extent.height}});
    command_buffer.CommandBindPipeline(*graphics_pipeline_);
    command_buffer.CommandBindVertexBuffer(vbo_);

    matrix_.projection_ = camera_.GetPerspectiveMatrix(), matrix_.view_ = camera_.GetLookAtMatrix(),
    matrix_.model_ = glm::rotate(Innsmouth::Matrix4f(1.0), rotation_.y, Innsmouth::Y_);

    command_buffer.CommandPushConstants(*graphics_pipeline_, Innsmouth::ShaderStage::VERTEX,
                                        std::as_bytes(Innsmouth::ToSpan(matrix_)));

    command_buffer.CommandDraw(mesh_.vertices_.size(), 1, 0, 0);
  }

private:
  Innsmouth::Vector3f rotation_;
  Innsmouth::Vector3f camera_position;
  Matrix matrix_;
  Innsmouth::Camera camera_;
  std::unique_ptr<Innsmouth::GraphicsPipeline> graphics_pipeline_;
  Innsmouth::Buffer vbo_;
  Innsmouth::Mesh mesh_;
};

int main() {

  MyApp app;

  app.Run();

  return 0;
}