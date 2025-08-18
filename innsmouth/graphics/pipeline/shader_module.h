#ifndef INNSMOUTH_SHADER_MODULE_H
#define INNSMOUTH_SHADER_MODULE_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"
#include <filesystem>

namespace Innsmouth {

class ShaderModule {
public:
  ShaderModule(const std::filesystem::path &shader_path);

  ~ShaderModule();

  const VkShaderModule GetShaderModule() const {
    return shader_module_;
  }

  VkShaderStageFlagBits GetShaderStage() const {
    return shader_stage_;
  }

  std::span<const VkPushConstantRange> GetPushConstantRanges() const {
    return push_constant_ranges_;
  }

  std::span<const VkVertexInputAttributeDescription> GetVertexInputAttributes() const {
    return input_attribute_descriptions_;
  }

  std::span<const VkVertexInputBindingDescription> GetVertexInputBinding() const {
    return input_binding_description_;
  }

protected:
  void ParseShader(std::span<const std::byte> shader_binary_data);

private:
  VkShaderStageFlagBits shader_stage_;
  VkShaderModule shader_module_{VK_NULL_HANDLE};
  std::vector<VkPushConstantRange> push_constant_ranges_;
  std::vector<VkVertexInputBindingDescription> input_binding_description_;
  std::vector<VkVertexInputAttributeDescription> input_attribute_descriptions_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_SHADER_MODULE_H