#include "innsmouth/application/include/innsmouth.h"
#include <iostream>

using namespace Innsmouth;

class Viewer : public Layer {
public:
  Viewer()
    : renderer_(Application::Get().GetSwapchain().GetSurfaceExtent().width,
                Application::Get().GetSwapchain().GetSurfaceExtent().height,
                Application::Get().GetSwapchain().GetSurfaceFormat()) {}

  void OnImGui() override {
    transform_.OnImGui();
    camera_.OnImGui();
    light_.OnImGui();
  }

  void OnSwapchain() override {
    auto &swapchain = Application::Get().GetSwapchain();
    renderer_.OnResize(swapchain.GetSurfaceWidth(), swapchain.GetSurfaceHeight());
  }

  void OnUpdate(CommandBuffer &command_buffer) override {
    camera_.OnUpdate();

    renderer_.Begin(command_buffer, light_.GetPointLights().front(), camera_.GetCamera());

    renderer_.DrawModel(command_buffer, model_);

    renderer_.End(command_buffer);
  }

  void OnAttach() override {
    light_.AddPointLight();
    model_.LoadWavefront(ROOT / "assets" / "wavefront" / "sponza1" / "sponza.obj");
    renderer_.PushModel(model_);
  }

private:
  CameraWidget camera_;
  TransformWidget transform_;
  LightWidget light_;
  Model model_;
  Renderer renderer_;
};

int main() {
  Application application("Viewer", 800, 600);

  Viewer viewer;

  application.AddLayer(&viewer);

  application.Run();

  return 0;
}