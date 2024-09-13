#include "innsmouth/renderer/include/renderer.h"
#include "innsmouth/application/include/application.h"
#include "innsmouth/core/include/tools.h"
#include "innsmouth/mesh/include/model.h"

namespace Innsmouth {

Renderer::Renderer(uint32_t width, uint32_t height, Format format)
  : vertex_buffer_(Innsmouth::BufferUsage::VERTEX_BUFFER, 200_MiB) {

  depth_image_ = std::make_unique<DepthImage>(width, height, Format::D32_SFLOAT_S8_UINT);

  graphics_pipeline_ = std::make_unique<GraphicsPipeline>(
    std::vector{SHADER_DIR / "mesh.vert.spv", SHADER_DIR / "mesh.frag.spv"}, std::vector<Format>{format},
    Format::D32_SFLOAT_S8_UINT);
}

void Renderer::DrawModel(CommandBuffer &command_buffer, const Model &model) {
  for (const auto &mesh : model.meshes_) {

    auto &ambient_name = mesh.material_.textures_[TextureType::AMBIENT];

    if (ambient_name.empty() == false) {
      auto write_descriptor = model.textures_.at(ambient_name).GetWriteDescriptorSet(0);

      command_buffer.CommandPushDescriptorSet(*graphics_pipeline_, 0,
                                              ToSpan(write_descriptor.write_descriptor_set_));
    }

    auto &diffuse_name = mesh.material_.textures_[TextureType::DIFFUSE];

    if (diffuse_name.empty() == false) {
      auto write_descriptor = model.textures_.at(diffuse_name).GetWriteDescriptorSet(1);

      command_buffer.CommandPushDescriptorSet(*graphics_pipeline_, 0,
                                              ToSpan(write_descriptor.write_descriptor_set_));
    }

    auto &specular_name = mesh.material_.textures_[TextureType::SPECULAR];

    if (specular_name.empty() == false) {
      auto write_descriptor = model.textures_.at(specular_name).GetWriteDescriptorSet(2);

      command_buffer.CommandPushDescriptorSet(*graphics_pipeline_, 0,
                                              ToSpan(write_descriptor.write_descriptor_set_));
    }

    command_buffer.CommandDraw(mesh.vertices_size_, 1, mesh.vertices_offset_, 0);
  }
}

void Renderer::Begin(CommandBuffer &command_buffer) {
  auto &swapchain = Application::Get().GetSwapchain();

  std::vector<RenderingAttachment> attachments{
    {LoadOperation::CLEAR, StoreOperation::STORE, swapchain.GetCurrentImageView()}};

  command_buffer.CommandBeginRendering(swapchain.GetSurfaceExtent(), attachments);
}

void Renderer::End(CommandBuffer &command_buffer) { command_buffer.CommandEndRendering(); }

void Renderer::OnResize(uint32_t width, uint32_t height) {
  depth_image_ = std::make_unique<DepthImage>(width, height, Format::D32_SFLOAT_S8_UINT);
}

} // namespace Innsmouth