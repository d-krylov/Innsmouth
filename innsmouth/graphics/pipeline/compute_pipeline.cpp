#include "compute_pipeline.h"
#include "innsmouth/graphics/include/graphics.h"
#include "shader_module.h"

namespace Innsmouth {

ComputePipeline::ComputePipeline(const std::filesystem::path &path) {
  ShaderModule shader(path);

  VkPipelineShaderStageCreateInfo shader_stage_ci{};
  {
    shader_stage_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_ci.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shader_stage_ci.module = shader;
    shader_stage_ci.pName = "main";
  }

  VkComputePipelineCreateInfo compute_pipeline_ci{};
  {
    compute_pipeline_ci.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    compute_pipeline_ci.layout = pipeline_layout_;
    compute_pipeline_ci.stage = shader_stage_ci;
  }
  VK_CHECK(vkCreateComputePipelines(Device(), VK_NULL_HANDLE, 1, &compute_pipeline_ci, nullptr,
                                    &pipeline_));
}

ComputePipeline::~ComputePipeline() {}

} // namespace Innsmouth