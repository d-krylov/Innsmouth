#ifndef INNSMOUTH_IMGUI_RENDERER_H
#define INNSMOUTH_IMGUI_RENDERER_H

#include "innsmouth/graphics/include/graphics_common.h"

struct ImDrawData;

namespace Innsmouth {

class ImGuiRenderer {
public:
  ImGuiRenderer(const Swapchain &swapchain);

  void RenderDrawData(CommandBuffer &command_buffer, ImDrawData *draw_data);
  void SetupRenderState(CommandBuffer &command_buffer, ImDrawData *draw_data);

  void Begin(CommandBuffer &command_buffer);
  void End(CommandBuffer &command_buffer);

  void CreateFontsTexture();
  void AddTexture();

private:
  GraphicsPipeline graphics_pipeline_;
  Buffer vertex_buffer_;
  Buffer index_buffer_;
  std::unique_ptr<Image2D> font_image_;
  WriteDescriptorSet descriptor_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMGUI_RENDERER_H