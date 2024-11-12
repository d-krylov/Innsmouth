#include "application/include/innsmouth.h"

using namespace Innsmouth;

class Test : public Layer {
public:
  Test()
    : graphics_pipeline_(std::vector{SHADER_DIR / "test.vert.spv", SHADER_DIR / "test.frag.spv"},
                         std::vector{Application::Get().GetSwapchain().GetSurfaceFormat()},
                         Format::D32_SFLOAT_S8_UINT),
      vertex_buffer_(BufferUsage::VERTEX_BUFFER, 100_MiB),
      uniform_buffer_(BufferUsage::UNIFORM_BUFFER, sizeof(Scene)) {}

  void OnImGui() override {
    transform_.OnImGui();
    camera_.OnImGui();
    light_.OnImGui();
  }

  void OnSwapchain() override {
    auto &swapchain = Application::Get().GetSwapchain();
    depth_image_ = std::make_unique<DepthImage>(swapchain.GetSurfaceWidth(), swapchain.GetSurfaceHeight(),
                                                Format::D32_SFLOAT_S8_UINT);
  }

  void OnUpdate(CommandBuffer &command_buffer) override {

    auto &swapchain = Application::Get().GetSwapchain();

    camera_.OnUpdate();

    std::vector<RenderingAttachment> attachments{
      {LoadOperation::CLEAR, StoreOperation::STORE, swapchain.GetCurrentImageView()}};

    auto &transform = transform_.GetTransform();
    push_constants_.projection_ = camera_.GetCamera().GetPerspectiveMatrix();
    push_constants_.view_ = camera_.GetCamera().GetLookAtMatrix();
    push_constants_.model_ = transform.GetModelMatrix();

    auto &extent = swapchain.GetSurfaceExtent();

    auto light = light_.GetPointLight();

    scene_.light_position_ = Vector4f(light.position_, 0.0f);
    scene_.light_color_ = Vector4f(light.color_, 0.0f);
    scene_.light_attenuation_ = Vector4f(light.attenuation_, 0.0f);
    scene_.camera_position_ = Vector4f(camera_.GetCamera().GetPosition(), 0.0f);

    uniform_buffer_.SetData(std::vector{scene_});

    command_buffer.CommandBeginRendering(
      extent, attachments,
      RenderingAttachment{LoadOperation::CLEAR, StoreOperation::STORE, depth_image_->GetImageView()});

    command_buffer.CommandBindPipeline(graphics_pipeline_);
    command_buffer.CommandBindVertexBuffer(vertex_buffer_);
    command_buffer.CommandSetCullMode(CullMode::BACK_BIT);
    command_buffer.CommandSetFrontFace(FrontFace::COUNTER_CLOCKWISE);
    command_buffer.CommandEnableDepthTest(true);
    command_buffer.CommandEnableDepthWrite(true);
    command_buffer.CommandSetViewport(0.0f, 0.0f, float(extent.width), float(extent.height));
    command_buffer.CommandSetScissor(0, 0, extent.width, extent.height);
    command_buffer.CommandPushConstants(graphics_pipeline_, ShaderStage::VERTEX, push_constants_);

    command_buffer.CommandPushDescriptorSet(graphics_pipeline_, 0, 0, uniform_buffer_, 0, sizeof(Scene));

    command_buffer.CommandDraw(model_.vertices_.size(), 1, 0, 0);

    command_buffer.CommandEndRendering();
  }

  void OnAttach() override {
    model_.LoadWavefront(ROOT / "assets" / "wavefront" / "sponza1" / "sponza.obj");

    auto &swapchain = Application::Get().GetSwapchain();
    auto &extent = swapchain.GetSurfaceExtent();

    depth_image_ = std::make_unique<DepthImage>(extent.width, extent.height, Format::D32_SFLOAT_S8_UINT);

    vertex_buffer_.SetData(model_.vertices_);
  }

private:
  struct PushConstants {
    Matrix4f projection_{1.0f};
    Matrix4f view_{1.0f};
    Matrix4f model_{1.0f};
  };

  struct Scene {
    Vector4f light_position_;
    Vector4f light_color_;
    Vector4f light_attenuation_;
    Vector4f camera_position_;
  };

private:
  GraphicsPipeline graphics_pipeline_;
  Buffer vertex_buffer_;
  Buffer uniform_buffer_;
  std::unique_ptr<DepthImage> depth_image_;
  PushConstants push_constants_;
  TransformWidget transform_;
  LightWidget light_;
  CameraWidget camera_;
  Scene scene_;
  Model model_;
};

int main() {

  Application application("Test", 800, 600);

  Test test;

  application.AddLayer(&test);

  application.Run();

  return 0;
}