#ifndef INNSMOUTH_RENDERER_H
#define INNSMOUTH_RENDERER_H

#include "innsmouth/graphics/include/graphics_common.h"

namespace Innsmouth {

class Model;
class CommandBuffer;

class Renderer {
public:
  Renderer(uint32_t width, uint32_t height, Format format);

  void DrawModel(CommandBuffer &command_buffer, const Model &model);

  void OnResize(uint32_t width, uint32_t height);

  void Begin(CommandBuffer &command_buffer);

  void End(CommandBuffer &command_buffer);

private:
  std::unique_ptr<GraphicsPipeline> graphics_pipeline_;
  std::unique_ptr<DepthImage> depth_image_;
  Buffer vertex_buffer_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_RENDERER_H