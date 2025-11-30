#ifndef INNSMOUTH_GRAPHICS_PIPELINE_H
#define INNSMOUTH_GRAPHICS_PIPELINE_H

#include "pipeline_tools.h"
#include <filesystem>

namespace Innsmouth {

struct GraphicsPipelineSpecification {
  std::vector<std::filesystem::path> shader_paths_;
  std::vector<DynamicState> dynamic_states_{DynamicState::E_VIEWPORT, DynamicState::E_SCISSOR};
  std::vector<Format> color_formats_;
  Format depth_format_ = Format::E_UNDEFINED;
  Format stencil_format_ = Format::E_UNDEFINED;
};

class GraphicsPipeline {
public:
  GraphicsPipeline() = default;

  GraphicsPipeline(const GraphicsPipelineSpecification &pipeline_specification);

  ~GraphicsPipeline();

  GraphicsPipeline(const GraphicsPipeline &) = delete;
  GraphicsPipeline &operator=(const GraphicsPipeline &) = delete;

  GraphicsPipeline(GraphicsPipeline &&other) noexcept;
  GraphicsPipeline &operator=(GraphicsPipeline &&other) noexcept;

  VkPipelineLayout GetPipelineLayout() const;
  VkPipeline GetPipeline() const;
  std::span<const VkDescriptorSetLayout> GetDescriptorSetLayouts() const;

private:
  VkPipeline graphics_pipeline_{VK_NULL_HANDLE};
  VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};
  std::vector<VkDescriptorSetLayout> descriptor_set_layouts_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_PIPELINE_H