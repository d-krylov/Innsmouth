#ifndef INNSMOUTH_PIPELINE_H
#define INNSMOUTH_PIPELINE_H

#include "innsmouth/graphics/include/graphics_types.h"

namespace Innsmouth {

struct VertexDescription {
  std::vector<VkVertexInputAttributeDescription> attributes;
  std::vector<VkVertexInputBindingDescription> bindings;
};

class ShaderModule;

class Pipeline {
protected:
  void CreatePipelineLayout(const std::vector<VkPushConstantRange> &push_constant_ranges);

protected:
  VkPipeline pipeline_{VK_NULL_HANDLE};
  VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};
  std::vector<VkDescriptorSetLayout> descriptor_set_layouts_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_PIPELINE_H