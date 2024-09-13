#ifndef INNSMOUTH_GRAPHICS_PIPELINE_H
#define INNSMOUTH_GRAPHICS_PIPELINE_H

#include "innsmouth/core/include/concepts.h"
#include "innsmouth/core/include/macros.h"
#include "pipeline.h"

namespace Innsmouth {

class ShaderModule;

class GraphicsPipeline : public Pipeline {
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
  [[nodiscard]] const std::vector<VkDescriptorSetLayout> &GetDescriptorSetLayouts() const {
    return descriptor_set_layouts_;
  }

  [[nodiscard]] const VkDescriptorSetLayout GetDescriptorSetLayout(uint32_t i) const {
    return descriptor_set_layouts_[i];
  }

protected:
  void ProcessDescriptorSets(const std::vector<ShaderModule> &modules);

private:
};

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_PIPELINE_H