#include "ray_tracing_pipeline.h"

namespace Innsmouth {

RayTracingPipeline::RayTracingPipeline() {

  // DYNAMIC STATES
  PipelineDynamicStateCreateInfo dynamic_state_ci;
  dynamic_state_ci.dynamicStateCount = {};
  dynamic_state_ci.pDynamicStates = {};

  RayTracingPipelineCreateInfoKHR ray_tracing_pipeline_ci;
  ray_tracing_pipeline_ci.pNext = {};
  ray_tracing_pipeline_ci.flags = {};
  ray_tracing_pipeline_ci.stageCount = {};
  ray_tracing_pipeline_ci.pStages = {};
  ray_tracing_pipeline_ci.groupCount = {};
  ray_tracing_pipeline_ci.pGroups = {};
  ray_tracing_pipeline_ci.maxPipelineRayRecursionDepth = {};
  ray_tracing_pipeline_ci.pLibraryInfo = {};
  ray_tracing_pipeline_ci.pLibraryInterface = {};
  ray_tracing_pipeline_ci.pDynamicState = &dynamic_state_ci;
  ray_tracing_pipeline_ci.layout = {};
  ray_tracing_pipeline_ci.basePipelineHandle = {};
  ray_tracing_pipeline_ci.basePipelineIndex = {};

  // VK_CHECK(vkCreateRayTracingPipelinesKHR(GraphicsContext::Get()->GetDevice(), ));
}

VkPipelineLayout RayTracingPipeline::GetPipelineLayout() const {
  return pipeline_layout_;
}

VkPipeline RayTracingPipeline::GetPipeline() const {
  return ray_tracing_pipeline_;
}

} // namespace Innsmouth