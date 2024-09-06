#ifndef INNSMOUTH_GRAPHICS_PIPELINE_H
#define INNSMOUTH_GRAPHICS_PIPELINE_H

#include "innsmouth/core/include/macros.h"
#include "innsmouth/graphics/include/graphics_types.h"

namespace Innsmouth {

class ShaderModule;

class GraphicsPipeline {
public:
  GraphicsPipeline(const std::vector<std::filesystem::path> &paths,
                   const std::vector<VkFormat> &color_formats, Depth depth = Depth::NONE,
                   VkFormat depth_format = VK_FORMAT_UNDEFINED,
                   const std::vector<VkVertexInputAttributeDescription> &vertex_attributes = {},
                   const std::vector<VkVertexInputBindingDescription> &vertex_bindings = {},
                   const std::vector<VkDynamicState> dynamic_states = GetDynamicStates(),
                   PrimitiveTopology topology = PrimitiveTopology::TRIANGLE_LIST);

  GraphicsPipeline();

  ~GraphicsPipeline();

  NO_COPY_SEMANTIC(GraphicsPipeline);

  operator const VkPipeline &() const { return graphics_pipeline_; }

  [[nodiscard]] const VkPipelineLayout GetPipelineLayout() const { return pipeline_layout_; }
  [[nodiscard]] const std::vector<VkDescriptorSetLayout> &GetDescriptorSetLayouts() const {
    return descriptor_set_layouts_;
  }

  [[nodiscard]] const VkDescriptorSetLayout GetDescriptorSetLayout(uint32_t i) const {
    return descriptor_set_layouts_[i];
  }

protected:
  void ProcessDescriptorSets(const std::vector<ShaderModule> &modules);

private:
  VkPipeline graphics_pipeline_{VK_NULL_HANDLE};
  VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};
  std::vector<VkDescriptorSetLayout> descriptor_set_layouts_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_PIPELINE_H