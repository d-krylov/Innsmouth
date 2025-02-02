#ifndef INNSMOUTH_GRAPHICS_PIPELINE_H
#define INNSMOUTH_GRAPHICS_PIPELINE_H

#include "shader_module.h"

namespace Innsmouth {

class GraphicsPipeline {
public:
  GraphicsPipeline(const std::filesystem::path &vertex_shader, const std::filesystem::path &fragment_shader, Format color_format);

  ~GraphicsPipeline();

  const VkPipelineLayout GetPipelineLayout() const { return pipeline_layout_; }

  operator const VkPipeline &() const { return graphics_pipeline_; }

  const VkPipeline *get() const { return &graphics_pipeline_; }

protected:
  void CreateGraphicsPipeline(std::span<const ShaderModule> shader_modules, std::span<const Format> color_formats,
                              const VkPipelineColorBlendAttachmentState &color_blend_attachment_state,
                              std::span<const VkVertexInputAttributeDescription> vertex_input_attributes,
                              std::span<const VkVertexInputBindingDescription> vertex_input_bindings);

private:
  VkPipeline graphics_pipeline_{VK_NULL_HANDLE};
  VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};
  std::vector<VkDescriptorSetLayout> descriptor_set_layouts_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_PIPELINE_H