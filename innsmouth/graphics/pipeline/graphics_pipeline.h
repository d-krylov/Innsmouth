#ifndef INNSMOUTH_GRAPHICS_PIPELINE_H
#define INNSMOUTH_GRAPHICS_PIPELINE_H

#include "graphics/include/graphics_types.h"
#include <filesystem>
#include <span>

namespace Innsmouth {

class ShaderModule;

class GraphicsPipeline {
public:
  GraphicsPipeline(std::span<const std::filesystem::path> paths, std::span<const Format> color_formats,
                   Format depth_format = Format::UNDEFINED,
                   const std::vector<VkVertexInputAttributeDescription> &vertex_attributes = {},
                   const std::vector<VkVertexInputBindingDescription> &vertex_bindings = {});

  ~GraphicsPipeline();

  NO_COPY_SEMANTIC(GraphicsPipeline);

  operator const VkPipeline &() const { return pipeline_; }

  [[nodiscard]] const VkPipelineLayout GetPipelineLayout() const { return pipeline_layout_; }

protected:
  void CreateGraphicsPipeline();

  void ProcessDescriptorSets(const std::vector<ShaderModule> &modules);

private:
  VkPipeline pipeline_{VK_NULL_HANDLE};
  VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};
  std::vector<VkDescriptorSetLayout> descriptor_set_layouts_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_PIPELINE_H