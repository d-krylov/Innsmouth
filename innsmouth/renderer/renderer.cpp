#include "innsmouth/renderer/include/renderer.h"
#include "innsmouth/mesh/include/model.h"

namespace Innsmouth {

Renderer::Renderer(VkFormat format)
  : vertex_buffer_(Innsmouth::BufferUsage::VERTEX_BUFFER, 200_MiB) {
  graphics_pipeline_ = std::make_unique<GraphicsPipeline>(
    std::vector<std::filesystem::path>{"./mesh.vert.spv", "./mesh.frag.spv"},
    std::vector<VkFormat>{format}, Innsmouth::Depth::READ_WRITE, depth_image_->GetFormat());
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

} // namespace Innsmouth