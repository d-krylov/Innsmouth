#include "ray_tracing_pipeline.h"
#include <cassert>
#include <print>

namespace Innsmouth {

void SetShaderGroupCreateInformation(RayTracingShaderGroupCreateInfoKHR &shader_group, ShaderStageMaskBits shader_stage, uint32_t shader_index) {
  switch (shader_stage) {
  case ShaderStageMaskBits::E_RAYGEN_BIT_KHR:
  case ShaderStageMaskBits::E_MISS_BIT_KHR:
  case ShaderStageMaskBits::E_CALLABLE_BIT_KHR: shader_group.generalShader = shader_index; break;
  case ShaderStageMaskBits::E_ANY_HIT_BIT_KHR: shader_group.anyHitShader = shader_index; break;
  case ShaderStageMaskBits::E_CLOSEST_HIT_BIT_KHR: shader_group.closestHitShader = shader_index; break;
  case ShaderStageMaskBits::E_INTERSECTION_BIT_KHR: shader_group.intersectionShader = shader_index; break;
  default: break;
  }
}

namespace {
ShaderStageMask general_mask =
  ShaderStageMaskBits::E_RAYGEN_BIT_KHR | ShaderStageMaskBits::E_MISS_BIT_KHR | ShaderStageMaskBits::E_CALLABLE_BIT_KHR;
ShaderStageMask hit_mask =
  ShaderStageMaskBits::E_ANY_HIT_BIT_KHR | ShaderStageMaskBits::E_CLOSEST_HIT_BIT_KHR | ShaderStageMaskBits::E_INTERSECTION_BIT_KHR;
} // namespace

auto GetRayTracingShaderGroups(std::span<const ShaderGroupPaths> shader_groups, std::span<const ShaderModule> shader_modules) {
  std::vector<RayTracingShaderGroupCreateInfoKHR> shader_group_cis;
  auto shader_index = 0;
  for (const auto &shader_group : shader_groups) {
    auto &shader_group_ci = shader_group_cis.emplace_back();
    shader_group_ci.generalShader = VK_SHADER_UNUSED_KHR;
    shader_group_ci.closestHitShader = VK_SHADER_UNUSED_KHR;
    shader_group_ci.anyHitShader = VK_SHADER_UNUSED_KHR;
    shader_group_ci.intersectionShader = VK_SHADER_UNUSED_KHR;
    ShaderStageMask shader_stage_mask;
    for (const auto &shader : shader_group) {
      auto shader_stage = shader_modules[shader_index].GetShaderStage();
      SetShaderGroupCreateInformation(shader_group_ci, shader_stage, shader_index++);
      shader_stage_mask |= shader_stage;
    }
    auto has_hit = shader_stage_mask.HasAnyBits(hit_mask);
    auto has_general = shader_stage_mask.HasAnyBits(general_mask);
    auto has_intersection = shader_stage_mask.HasBits(ShaderStageMaskBits::E_INTERSECTION_BIT_KHR);
    assert(has_hit == false || has_general == false);
    shader_group_ci.type = has_general ? RayTracingShaderGroupTypeKHR::E_GENERAL_KHR
                                       : (has_intersection ? RayTracingShaderGroupTypeKHR::E_PROCEDURAL_HIT_GROUP_KHR
                                                           : RayTracingShaderGroupTypeKHR::E_TRIANGLES_HIT_GROUP_KHR);
  }
  return shader_group_cis;
}

VkPipeline CreateRayTracingPipeline(std::span<const ShaderGroupPaths> shader_groups, std::span<const ShaderModule> shader_modules,
                                    VkPipelineLayout pipeline_layout, uint32_t maximum_recursion_depth) {

  std::vector<ShaderModuleCreateInfo> shader_modules_cis(shader_modules.size());
  std::vector<PipelineShaderStageCreateInfo> shader_stages_cis(shader_modules.size());

  for (auto i = 0; i < shader_modules.size(); i++) {
    shader_modules_cis[i].codeSize = shader_modules[i].GetSize();
    shader_modules_cis[i].pCode = shader_modules[i].GetBinaryData().data();
    shader_stages_cis[i].stage = shader_modules[i].GetShaderStage();
    shader_stages_cis[i].pNext = &shader_modules_cis[i];
    shader_stages_cis[i].pName = "main";
  }

  // DYNAMIC STATES
  PipelineDynamicStateCreateInfo dynamic_state_ci;

  auto ray_tracing_shader_group_cis = GetRayTracingShaderGroups(shader_groups, shader_modules);

  RayTracingPipelineCreateInfoKHR ray_tracing_pipeline_ci;
  ray_tracing_pipeline_ci.stageCount = shader_stages_cis.size();
  ray_tracing_pipeline_ci.pStages = shader_stages_cis.data();
  ray_tracing_pipeline_ci.groupCount = ray_tracing_shader_group_cis.size();
  ray_tracing_pipeline_ci.pGroups = ray_tracing_shader_group_cis.data();
  ray_tracing_pipeline_ci.maxPipelineRayRecursionDepth = maximum_recursion_depth;
  ray_tracing_pipeline_ci.pDynamicState = &dynamic_state_ci;
  ray_tracing_pipeline_ci.layout = pipeline_layout;

  VkPipeline ray_tracing_pipeline = VK_NULL_HANDLE;
  VK_CHECK(vkCreateRayTracingPipelinesKHR(GraphicsContext::Get()->GetDevice(), VK_NULL_HANDLE, VK_NULL_HANDLE, 1, ray_tracing_pipeline_ci,
                                          nullptr, &ray_tracing_pipeline));

  return ray_tracing_pipeline;
}

RayTracingPipeline::RayTracingPipeline(std::vector<ShaderGroupPaths> shader_groups, uint32_t maximum_recursion_depth) {
  std::vector<ShaderModule> shader_modules;
  for (const auto &shader_group : shader_groups) {
    shader_modules.insert(shader_modules.end(), shader_group.begin(), shader_group.end());
  }

  descriptor_set_layouts_ = CreateDescriptorSetLayouts(shader_modules);
  pipeline_layout_ = CreatePipelineLayout(descriptor_set_layouts_, {});
  ray_tracing_pipeline_ = CreateRayTracingPipeline(shader_groups, shader_modules, pipeline_layout_, maximum_recursion_depth);
}

VkPipelineLayout RayTracingPipeline::GetPipelineLayout() const {
  return pipeline_layout_;
}

VkPipeline RayTracingPipeline::GetPipeline() const {
  return ray_tracing_pipeline_;
}

RayTracingPipeline::RayTracingPipeline(RayTracingPipeline &&other) noexcept {
  ray_tracing_pipeline_ = std::exchange(other.ray_tracing_pipeline_, VK_NULL_HANDLE);
  pipeline_layout_ = std::exchange(other.pipeline_layout_, VK_NULL_HANDLE);
  descriptor_set_layouts_ = std::move(other.descriptor_set_layouts_);
}

RayTracingPipeline &RayTracingPipeline::operator=(RayTracingPipeline &&other) noexcept {
  std::swap(ray_tracing_pipeline_, other.ray_tracing_pipeline_);
  std::swap(pipeline_layout_, other.pipeline_layout_);
  std::swap(descriptor_set_layouts_, other.descriptor_set_layouts_);
  return *this;
}

} // namespace Innsmouth