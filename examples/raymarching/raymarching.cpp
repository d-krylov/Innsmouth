#include "imgui.h"
#include "innsmouth/application/include/application.h"
#include "innsmouth/core/include/tools.h"
#include "innsmouth/mesh/include/primitives.h"
#include "innsmouth/scene/include/camera.h"

using namespace Innsmouth;

class Raymarching : public Layer {
public:
  Raymarching() {}

  void OnImGui() override {
    ImGui::Begin("Settings");

    ImGui::End();
  }

  void OnUpdate(Innsmouth::CommandBuffer &command_buffer) override {
    command_buffer.CommandSetPrimitiveTopology(PrimitiveTopology::TRIANGLE_LIST);
  }

private:
};

int main() {
  Application application("Raymarching", 800, 600);
  Raymarching raymarching;

  application.AddLayer(&raymarching);
  application.Run();

  return 0;
}