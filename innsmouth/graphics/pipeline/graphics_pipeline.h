#ifndef INNSMOUTH_GRAPHICS_PIPELINE_H
#define INNSMOUTH_GRAPHICS_PIPELINE_H

#include "innsmouth/core/include/concepts.h"
#include "innsmouth/core/include/macros.h"
#include "pipeline.h"

namespace Innsmouth {

class ShaderModule;

class GraphicsPipeline {
public:
  GraphicsPipeline(const std::vector<std::filesystem::path> &paths, const std::vector<Format> &color_formats,
                   Format depth_format = Format::UNDEFINED,
                   const std::vector<VkVertexInputAttributeDescription> &vertex_attributes = {},
                   const std::vector<VkVertexInputBindingDescription> &vertex_bindings = {},
                   const std::vector<VkDynamicState> dynamic_states = GetDynamicStates());

  ~GraphicsPipeline();

  NO_COPY_SEMANTIC(GraphicsPipeline);

  operator const VkPipeline &() const { return pipeline_; }

  [[nodiscard]] const VkPipelineLayout GetPipelineLayout() const { return pipeline_layout_; }

protected:
  void ProcessDescriptorSets(const std::vector<ShaderModule> &modules);

private:
  VkPipeline pipeline_{VK_NULL_HANDLE};
  VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};
  std::vector<VkDescriptorSetLayout> descriptor_set_layouts_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_PIPELINE_H