#ifndef INNSMOUTH_IMGUI_RENDERER_H
#define INNSMOUTH_IMGUI_RENDERER_H

#include "graphics/buffer/buffer.h"
#include "graphics/image/image2D.h"
#include "graphics/pipeline/graphics_pipeline.h"

struct ImDrawData;

namespace Innsmouth {

class CommandBuffer;

class ImGuiRenderer {
public:
  ImGuiRenderer(Format color_format);

  void RenderDrawData(CommandBuffer &command_buffer);

  void Begin(CommandBuffer &command_buffer);
  void End(CommandBuffer &command_buffer);

protected:
  struct GuiPushConstants {
    float scale_x;
    float scale_y;
    float translate_x;
    float translate_y;
  };

  void SetBuffers(ImDrawData *draw_data);
  void SetupRenderState(CommandBuffer &command_buffer, ImDrawData *draw_data);
  void CreateFontsTexture();

private:
  GraphicsPipeline graphics_pipeline_;
  Buffer vertex_buffer_;
  Buffer index_buffer_;
  GuiPushConstants gui_push_constants_;
  std::unique_ptr<Image2D> font_image_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_IMGUI_RENDERER_H