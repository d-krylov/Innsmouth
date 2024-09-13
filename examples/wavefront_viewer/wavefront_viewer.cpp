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
  }

  bool OnWindowSizeEvent(const WindowSizeEvent &event) {
    // renderer_.OnResize(event.GetWidth(), event.GetHeight());
    return true;
  }

  void OnEvent(Event &event) override {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowSizeEvent>(BIND_FUNCTION(Viewer::OnWindowSizeEvent));
  }

  void OnUpdate(Innsmouth::CommandBuffer &command_buffer) override {

    renderer_.Begin(command_buffer);

    camera_.OnUpdate();

    renderer_.End(command_buffer);
  }

  void OnAttach() override { model_.LoadWavefront(ROOT / "assets" / "Sponza-master" / "sponza.obj"); }

private:
  CameraWidget camera_;
  TransformWidget transform_;
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