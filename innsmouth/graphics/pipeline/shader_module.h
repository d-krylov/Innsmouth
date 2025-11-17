#ifndef INNSMOUTH_SHADER_MODULE_H
#define INNSMOUTH_SHADER_MODULE_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"
#include <filesystem>
#include <map>

namespace Innsmouth {

using DescriptorSetLayoutBindingMap = std::map<uint32_t, std::vector<DescriptorSetLayoutBinding>>;

class ShaderModule {
public:
  ShaderModule(const std::filesystem::path &shader_path);

  std::size_t GetSize() const;
  std::span<const uint32_t> GetBinaryData() const;
  ShaderStageMaskBits GetShaderStage() const;

  std::span<const PushConstantRange> GetPushConstantRanges() const;
  std::span<const VertexInputAttributeDescription> GetVertexInputAttributes() const;
  const DescriptorSetLayoutBindingMap &GetPushDescriptorSetLayoutBindings() const;
  const DescriptorSetLayoutBindingMap &GetPullDescriptorSetLayoutBindings() const;

protected:
  void ParseShader(std::span<const uint32_t> shader_binary_data);

private:
  std::vector<uint32_t> spirv_;
  ShaderStageMaskBits shader_stage_;
  std::vector<PushConstantRange> push_constant_ranges_;
  std::vector<VertexInputAttributeDescription> input_attribute_descriptions_;
  DescriptorSetLayoutBindingMap push_descriptor_set_bindings_;
  DescriptorSetLayoutBindingMap pull_descriptor_set_bindings_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_SHADER_MODULE_H