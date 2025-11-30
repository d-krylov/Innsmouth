#ifndef INNSMOUTH_PIPELINE_TOOLS_H
#define INNSMOUTH_PIPELINE_TOOLS_H

#include "shader_module.h"

namespace Innsmouth {

VkPipelineLayout CreatePipelineLayout(std::span<const VkDescriptorSetLayout> set_layouts, std::span<const PushConstantRange> push_constants);
std::vector<VkDescriptorSetLayout> CreateDescriptorSetLayouts(std::span<const ShaderModule> shader_modules);

} // namespace Innsmouth

#endif // INNSMOUTH_PIPELINE_TOOLS_H