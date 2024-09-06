#include "innsmouth/application/include/innsmouth.h"

using namespace Innsmouth;

class Viewer : public Layer {
public:
  Viewer() {}

  void OnImGui() override {
    transform_.OnImGui();
    camera_.OnImGui();
  }

  void OnUpdate(Innsmouth::CommandBuffer &command_buffer) override { camera_.OnUpdate(); }

  void OnAttach() override {
    model_.LoadWavefront(ROOT / "assets" / "Sponza-master" / "sponza.obj");
  }

private:
  CameraWidget camera_;
  TransformWidget transform_;
  Model model_;
};

int main() {
  Application application("Viewer", 800, 600);

  Viewer viewer;

  application.AddLayer(&viewer);

  application.Run();

  return 0;
}