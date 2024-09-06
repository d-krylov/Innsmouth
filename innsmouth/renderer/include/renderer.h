#ifndef INNSMOUTH_RENDERER_H
#define INNSMOUTH_RENDERER_H

#include "innsmouth/graphics/include/graphics_common.h"

namespace Innsmouth {

class Model;
class CommandBuffer;

class Renderer {
public:
  Renderer(VkFormat format);

  void DrawModel(CommandBuffer &command_buffer, const Model &model);

private:
  std::unique_ptr<GraphicsPipeline> graphics_pipeline_;
  std::unique_ptr<DepthImage> depth_image_;
  Buffer vertex_buffer_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_RENDERER_H