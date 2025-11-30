#ifndef INNSMOUTH_RAY_TRACING_PIPELINE_H
#define INNSMOUTH_RAY_TRACING_PIPELINE_H

#include "pipeline_tools.h"

namespace Innsmouth {

using ShaderGroupPaths = std::vector<std::filesystem::path>;

struct ShaderGroupCounts {
  uint32_t raygen_ = 0;
  uint32_t miss_ = 0;
  uint32_t hit_ = 0;
  uint32_t callable_ = 0;
};

class RayTracingPipeline {
public:
  RayTracingPipeline() = default;

  RayTracingPipeline(std::vector<ShaderGroupPaths> shader_groups, uint32_t maximum_recursion_depth = 1);

  RayTracingPipeline(const RayTracingPipeline &) = delete;
  RayTracingPipeline &operator=(const RayTracingPipeline &) = delete;

  RayTracingPipeline(RayTracingPipeline &&other) noexcept;
  RayTracingPipeline &operator=(RayTracingPipeline &&other) noexcept;

  VkPipelineLayout GetPipelineLayout() const;
  VkPipeline GetPipeline() const;

private:
  VkPipeline ray_tracing_pipeline_{VK_NULL_HANDLE};
  VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};
  std::vector<VkDescriptorSetLayout> descriptor_set_layouts_;
  ShaderGroupCounts shader_group_counts_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_RAY_TRACING_PIPELINE_H