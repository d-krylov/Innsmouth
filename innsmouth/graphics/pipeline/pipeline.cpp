#include "pipeline.h"
#include "innsmouth/graphics/include/graphics.h"
#include "shader_module.h"

namespace Innsmouth {

void Pipeline::CreatePipelineLayout(const std::vector<VkPushConstantRange> &push_constant_ranges) {
  VkPipelineLayoutCreateInfo pipeline_layout_ci{};
  {
    pipeline_layout_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_ci.setLayoutCount = descriptor_set_layouts_.size();
    pipeline_layout_ci.pSetLayouts = descriptor_set_layouts_.data();
    pipeline_layout_ci.pPushConstantRanges = push_constant_ranges.data();
    pipeline_layout_ci.pushConstantRangeCount = push_constant_ranges.size();
  }
  VK_CHECK(vkCreatePipelineLayout(Device(), &pipeline_layout_ci, nullptr, &pipeline_layout_));
}

} // namespace Innsmouth