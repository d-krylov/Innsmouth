#ifndef INNSMOUTH_IMGUI_RENDERER_H
#define INNSMOUTH_IMGUI_RENDERER_H

#include "innsmouth/graphics/buffer/buffer.h"
#include "innsmouth/graphics/image/image.h"
#include "innsmouth/graphics/pipeline/graphics_pipeline.h"

namespace Innsmouth {

class CommandBuffer;
class Swapchain;

class ImGuiRenderer {
public:
  ImGuiRenderer(Format color_format);

  void RenderDrawData(CommandBuffer &command_buffer);

  void Begin(CommandBuffer &command_buffer, const Swapchain &swapchain);
  void End(CommandBuffer &command_buffer);

protected:
  struct GuiPushConstants {
    float scale_x;
    float scale_y;
    float translate_x;
    float translate_y;
  };

  void SetBuffers();
  void SetupRenderState(CommandBuffer &command_buffer);
  void CreateFontsTexture();

private:
  GraphicsPipeline graphics_pipeline_;
  Buffer vertex_buffer_;
  Buffer index_buffer_;
  GuiPushConstants gui_push_constants_;
  Image font_image_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMGUI_RENDERER_H