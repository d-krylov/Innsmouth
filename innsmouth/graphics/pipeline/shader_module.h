#ifndef INNSMOUTH_SHADER_MODULE_H
#define INNSMOUTH_SHADER_MODULE_H

#include "graphics/include/graphics_types.h"
#include <filesystem>
#include <unordered_map>

class SpvReflectShaderModule;

namespace Innsmouth {

class ShaderModule {
public:
  ShaderModule(const std::filesystem::path &path);

  ~ShaderModule();

  NO_COPY_SEMANTIC(ShaderModule);

  [[nodiscard]] const std::vector<VkPushConstantRange> &GetPushConstantRanges() const {
    return push_constant_ranges_;
  }

  [[nodiscard]] const std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> &
  GetDescriptorSetData() const {
    return descriptor_map_;
  }

  [[nodiscard]] const std::vector<VkVertexInputAttributeDescription> &GetVertexInputAttributes() const {
    return inputs_;
  }

  [[nodiscard]] const VkVertexInputBindingDescription &GetVertexInputBinding() const {
    return binding_description_;
  }

  [[nodiscard]] VkShaderStageFlags GetStage() const { return shader_stage_; }

  operator const VkShaderModule &() const { return shader_module_; }

protected:
  void GetShaderInformation(const std::vector<std::byte> &data);

  void GetDescriptorSets(const SpvReflectShaderModule &module, VkShaderStageFlags stage);

private:
  VkShaderStageFlags shader_stage_;
  std::vector<VkPushConstantRange> push_constant_ranges_;
  std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> descriptor_map_;
  std::vector<VkVertexInputAttributeDescription> inputs_;
  VkVertexInputBindingDescription binding_description_{};
  VkShaderModule shader_module_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_SHADER_MODULE_H