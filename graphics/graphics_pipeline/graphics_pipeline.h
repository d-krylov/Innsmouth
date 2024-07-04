#ifndef INNSMOUTH_GRAPHICS_PIPELINE_H
#define INNSMOUTH_GRAPHICS_PIPELINE_H

#include "graphics/include/graphics_types.h"
#include <vector>

namespace Innsmouth {

class ShaderModule;

struct GraphicsPipelineDescription {
  VkPrimitiveTopology topology_{VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
  std::vector<std::filesystem::path> paths_;
  std::vector<VkFormat> color_formats_;
  std::vector<VkVertexInputAttributeDescription> vertex_inputs_;
  std::vector<VkVertexInputBindingDescription> vertex_bindings_;
};

class GraphicsPipeline {
public:
  GraphicsPipeline(const GraphicsPipelineDescription &description);

  ~GraphicsPipeline();

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