#ifndef INNSMOUTH_RENDERER_H
#define INNSMOUTH_RENDERER_H

#include "graphics/include/graphics_common.h"
#include "scene/include/light.h"

namespace Innsmouth {

class Model;
class CommandBuffer;
class Camera;
class PointLight;

class Renderer {
public:
  Renderer(uint32_t width, uint32_t height, Format format);

  void DrawModel(CommandBuffer &command_buffer, const Model &model);

  void PushModel(const Model &model);

  void OnResize(uint32_t width, uint32_t height);

  void Begin(CommandBuffer &command_buffer, const PointLight &light, const Camera &camera);

  void End(CommandBuffer &command_buffer);

private:
  struct PushConstants {
    Matrix4f projection_;
    Matrix4f view_;
    Matrix4f model_;
  };

  struct UniformBufferData {
    Vector4f light_position_{0.0f, 0.0f, 0.0f, 0.0f};
    Vector4f light_color_{0.0f, 0.0f, 0.0f, 0.0f};
    Vector4f light_attenuation_{0.0f, 0.0f, 1.0f, 0.0f};
    Vector4f camera_position_;
  };

private:
  std::unique_ptr<GraphicsPipeline> graphics_pipeline_;
  std::unique_ptr<DepthImage> depth_image_;
  Buffer vertex_buffer_;
  Buffer uniform_buffer_;
  PushConstants push_constants_;
  UniformBufferData uniform_buffer_data;
};

} // namespace Innsmouth

#endif // INNSMOUTH_RENDERER_H