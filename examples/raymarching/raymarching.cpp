#include "core/include/tools.h"
#include "imgui.h"
#include "innsmouth/include/application.h"
#include "mesh/include/primitives.h"
#include "scene/include/camera.h"

using namespace Innsmouth;

struct PushConstants {
  Vector2f i_mouse;
  Vector2f i_resolution_;
  float i_time_;
};

class Raymarching : public Application {
public:
  Raymarching()
    : Application("Raymarching", 800, 600),
      graphics_pipeline_({"./canvas.vert.spv", "./raytracing.frag.spv"},
                         {swapchain_->GetSurfaceFormat().format}) {
    std::vector<std::filesystem::path> image_paths{
      GetRoot() / "assets" / "images" / "shadertoy" / "black_stone.png",
      GetRoot() / "assets" / "images" / "container.png",
      GetRoot() / "assets" / "images" / "shadertoy" / "color_noise.png",
    };

    images_ = std::vector<Image2D>(image_paths.begin(), image_paths.end());

    std::vector<VkDescriptorImageInfo> ii;

    for (auto &image : images_) {
      VkDescriptorImageInfo descriptor_ii{};
      {
        descriptor_ii.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptor_ii.imageView = image.GetImageView();
        descriptor_ii.sampler = image.GetSampler();
      }
      ii.emplace_back(descriptor_ii);
    }

    descriptor_set_ = Image::GetWriteDescriptorSet(ii, 0);
  }

  void OnImGui() override {
    ImGui::Begin("Settings");
    cursor_ = ImGui::GetMousePos();
    ImGui::Text("Coord %f %f", cursor_.x, cursor_.y);
    ImGui::End();
  }

  void OnUpdate(Innsmouth::CommandBuffer &command_buffer) override {
    auto extent = swapchain_->GetSurfaceCapabilities().currentExtent;
    command_buffer.CommandSetViewport(0.0f, 0.0f, (float)extent.width, (float)extent.height);
    command_buffer.CommandSetScissor({{0, 0}, {extent.width, extent.height}});
    command_buffer.CommandBindPipeline(graphics_pipeline_);

    PushConstants pc;
    pc.i_mouse = Vector2f(cursor_.x, cursor_.y);
    pc.i_resolution_ = Vector2f(extent.width, extent.height);
    pc.i_time_ = GetTime();
    command_buffer.CommandPushConstants(graphics_pipeline_, ShaderStage::FRAGMENT,
                                        std::as_bytes(ToSpan(pc)));

    command_buffer.CommandPushDescriptorSet(graphics_pipeline_, 0,
                                            ToSpan(descriptor_set_.write_descriptor_set_));

    command_buffer.CommandDraw(3, 1, 0, 0);
  }

private:
  ImVec2 cursor_;
  GraphicsPipeline graphics_pipeline_;
  std::vector<Image2D> images_;
  WriteDescriptorSet descriptor_set_;
};

int main() {

  Raymarching app;

  app.Run();

  return 0;
}