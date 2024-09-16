#include "innsmouth/renderer/include/renderer.h"
#include "innsmouth/application/include/application.h"
#include "innsmouth/core/include/tools.h"
#include "innsmouth/mesh/include/model.h"
#include "innsmouth/scene/include/camera.h"
#include <iostream>

namespace Innsmouth {

Renderer::Renderer(uint32_t width, uint32_t height, Format format)
  : vertex_buffer_(BufferUsage::VERTEX_BUFFER, 200_MiB),
    uniform_buffer_(BufferUsage::UNIFORM_BUFFER, sizeof(UniformBufferData)) {

  depth_image_ = std::make_unique<DepthImage>(width, height, Format::D32_SFLOAT_S8_UINT);

  graphics_pipeline_ = std::make_unique<GraphicsPipeline>(
    std::vector{SHADER_DIR / "mesh.vert.spv", SHADER_DIR / "mesh.frag.spv"}, std::vector<Format>{format},
    Format::D32_SFLOAT_S8_UINT);
}

void Renderer::DrawModel(CommandBuffer &command_buffer, const Model &model) {
  for (const auto &mesh : model.meshes_) {
    auto &ambient_image = model.textures_.at(mesh.material_.names_.ambient_);
    command_buffer.CommandPushDescriptorSet(*graphics_pipeline_, 0, 0, ambient_image);
    auto &diffuse_image = model.textures_.at(mesh.material_.names_.diffuse_);
    command_buffer.CommandPushDescriptorSet(*graphics_pipeline_, 0, 1, diffuse_image);
    auto &specular_image = model.textures_.at(mesh.material_.names_.specular_);
    command_buffer.CommandPushDescriptorSet(*graphics_pipeline_, 0, 2, specular_image);
    command_buffer.CommandDraw(mesh.vertices_size_, 1, mesh.vertices_offset_, 0);
  }
}

void Renderer::Begin(CommandBuffer &command_buffer, const PointLight &light, const Camera &camera) {
  auto &swapchain = Application::Get().GetSwapchain();

  std::vector<RenderingAttachment> attachments{
    {LoadOperation::CLEAR, StoreOperation::STORE, swapchain.GetCurrentImageView()}};

  auto &extent = swapchain.GetSurfaceExtent();

  push_constants_.projection_ = camera.GetPerspectiveMatrix();
  push_constants_.view_ = camera.GetLookAtMatrix();
  push_constants_.model_ = Matrix4f(1.0f);

  command_buffer.CommandBeginRendering(
    extent, attachments,
    RenderingAttachment{LoadOperation::CLEAR, StoreOperation::STORE, depth_image_->GetImageView()});

  command_buffer.CommandBindPipeline(*graphics_pipeline_);
  command_buffer.CommandPushConstants(*graphics_pipeline_, ShaderStage::VERTEX, push_constants_);
  command_buffer.CommandBindVertexBuffer(vertex_buffer_);
  command_buffer.CommandSetCullMode(CullMode::BACK_BIT);
  command_buffer.CommandSetFrontFace(FrontFace::COUNTER_CLOCKWISE);
  command_buffer.CommandEnableDepthTest(true);
  command_buffer.CommandEnableDepthWrite(true);

  command_buffer.CommandSetViewport(0.0f, 0.0f, float(extent.width), float(extent.height));
  command_buffer.CommandSetScissor(0, 0, extent.width, extent.height);

  uniform_buffer_data.light_position_ = Vector4f(light.position_, 0.0f);
  uniform_buffer_data.light_color_ = Vector4f(light.color_, 0.0f);
  uniform_buffer_data.light_attenuation_ = Vector4f(light.attenuation_, 0.0f);
  uniform_buffer_data.camera_position_ = Vector4f(camera.GetPosition(), 0.0f);

  uniform_buffer_.SetData(std::vector{uniform_buffer_data});

  command_buffer.CommandPushDescriptorSet(*graphics_pipeline_, 0, 3, uniform_buffer_, 0,
                                          sizeof(uniform_buffer_data));
}

void Renderer::End(CommandBuffer &command_buffer) { command_buffer.CommandEndRendering(); }

void Renderer::OnResize(uint32_t width, uint32_t height) {
  depth_image_ = std::make_unique<DepthImage>(width, height, Format::D32_SFLOAT_S8_UINT);
}

void Renderer::PushModel(const Model &model) { vertex_buffer_.SetData(model.vertices_); }

} // namespace Innsmouth