#ifndef INNSMOUTH_SHADER_MODULE_H
#define INNSMOUTH_SHADER_MODULE_H

#include "graphics/include/graphics_types.h"
#include <filesystem>
#include <span>
#include <vector>

namespace Innsmouth {

class ShaderModule {
public:
  ShaderModule(const std::filesystem::path &path);

  ~ShaderModule();

  std::span<const VkPushConstantRange> GetPushConstantRanges() const { return push_constant_ranges_; }
  const VkShaderModule GetShaderModule() const { return shader_module_; }
  VkShaderStageFlagBits GetShaderStage() const { return shader_stage_; }

  std::span<const std::vector<VkDescriptorSetLayoutBinding>> GetDescriptorSetLayoutBindings() const { return descriptor_set_bindings_; }
  std::span<const VkVertexInputAttributeDescription> GetVertexInputAttributes() const { return input_attribute_descriptions_; }
  std::span<const VkVertexInputBindingDescription> GetVertexInputBinding() const { return input_binding_description_; }

protected:
  void ParseShader(std::span<const std::byte> data);

private:
  VkShaderStageFlagBits shader_stage_;
  VkShaderModule shader_module_{VK_NULL_HANDLE};
  std::vector<VkPushConstantRange> push_constant_ranges_;
  std::vector<VkVertexInputBindingDescription> input_binding_description_;
  std::vector<VkVertexInputAttributeDescription> input_attribute_descriptions_;
  std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptor_set_bindings_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_SHADER_MODULE_H