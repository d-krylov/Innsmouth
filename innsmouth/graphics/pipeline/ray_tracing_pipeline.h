#ifndef INNSMOUTH_RAY_TRACING_PIPELINE_H
#define INNSMOUTH_RAY_TRACING_PIPELINE_H

#include "shader_module.h"

namespace Innsmouth {

class RayTracingPipeline {
public:
  RayTracingPipeline();

  VkPipelineLayout GetPipelineLayout() const;
  VkPipeline GetPipeline() const;

private:
  VkPipeline ray_tracing_pipeline_{VK_NULL_HANDLE};
  VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_RAY_TRACING_PIPELINE_H