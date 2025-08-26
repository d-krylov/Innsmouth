#include "shader_module.h"
#include "SPIRV-Reflect/spirv_reflect.h"
#include <vulkan/utility/vk_format_utils.h>
#include <algorithm>
#include <numeric>
#include <fstream>

namespace Innsmouth {

std::vector<std::byte> ReadBinaryFile(const std::filesystem::path &path) {
  auto size = std::filesystem::file_size(path);
  std::vector<std::byte> buffer(size);
  std::ifstream input_file(path.string(), std::ios_base::binary);
  input_file.read(reinterpret_cast<char *>(buffer.data()), size);
  return buffer;
}

std::vector<VkVertexInputAttributeDescription> GetShaderInputs(const SpvReflectShaderModule &module) {
  auto offset = 0;
  auto inputs = Enumerate(spvReflectEnumerateInputVariables, &module);
  auto result = std::vector<VkVertexInputAttributeDescription>();
  std::ranges::sort(inputs, std::less(), &SpvReflectInterfaceVariable::location);
  for (const auto &input : inputs) {
    if (HasBits(input->decoration_flags, SPV_REFLECT_DECORATION_BUILT_IN)) {
      continue;
    }
    result.emplace_back(input->location, 0, VkFormat(input->format), offset);
    offset += vkuFormatElementSize(VkFormat(input->format));
  }
  return result;
}

auto GetStride(std::span<const VkVertexInputAttributeDescription> attributes) {
  auto stride = 0;
  for (const auto &attribute : attributes) {
    stride += vkuFormatElementSize(VkFormat(attribute.format));
  }
  return stride;
}

auto GetDescriptorSetBindings(const SpvReflectShaderModule &module, VkShaderStageFlags stage) {
  auto spv_descriptor_sets = Enumerate(spvReflectEnumerateDescriptorSets, &module);
  std::vector<std::vector<VkDescriptorSetLayoutBinding>> vk_sets_bindings;
  for (const auto &spv_descriptor_set : spv_descriptor_sets) {
    std::span<SpvReflectDescriptorBinding *> spv_set_bindings(spv_descriptor_set->bindings, spv_descriptor_set->binding_count);
    std::vector<VkDescriptorSetLayoutBinding> vk_bindings;
    for (const auto &spv_binding : spv_set_bindings) {
      std::span<uint32_t> dimensions(spv_binding->array.dims, spv_binding->array.dims_count);
      auto descriptor_count = std::accumulate(dimensions.begin(), dimensions.end(), 1u, std::multiplies<>());
      VkDescriptorSetLayoutBinding descriptor_set_binding{};
      {
        descriptor_set_binding.binding = spv_binding->binding;
        descriptor_set_binding.descriptorCount = descriptor_count;
        descriptor_set_binding.descriptorType = static_cast<VkDescriptorType>(spv_binding->descriptor_type);
        descriptor_set_binding.stageFlags = stage;
      }
      vk_bindings.emplace_back(descriptor_set_binding);
    }
    vk_sets_bindings.emplace_back(std::move(vk_bindings));
  }
  return vk_sets_bindings;
}

std::vector<VkPushConstantRange> GetPushConstants(const SpvReflectShaderModule &module, VkShaderStageFlagBits stage) {
  auto reflect_blocks = Enumerate(spvReflectEnumeratePushConstantBlocks, &module);
  auto push_constants = std::vector<VkPushConstantRange>();
  for (const auto &reflect_block : reflect_blocks) {
    push_constants.emplace_back(stage, reflect_block->absolute_offset, reflect_block->size);
  }
  return push_constants;
}

void ShaderModule::ParseShader(std::span<const std::byte> shader_binary_data) {
  SpvReflectShaderModule spv_module;
  auto status = spvReflectCreateShaderModule(shader_binary_data.size(), shader_binary_data.data(), &spv_module);
  shader_stage_ = static_cast<VkShaderStageFlagBits>(spv_module.shader_stage);
  input_attribute_descriptions_ = GetShaderInputs(spv_module);
  push_constant_ranges_ = GetPushConstants(spv_module, shader_stage_);
  descriptor_set_bindings_ = GetDescriptorSetBindings(spv_module, shader_stage_);
  auto stride = GetStride(input_attribute_descriptions_);
  if (input_attribute_descriptions_.size() > 0) {
    input_binding_description_.emplace_back(0, stride);
  }
}

ShaderModule::ShaderModule(const std::filesystem::path &shader_path) {
  auto shader_binary_content = ReadBinaryFile(shader_path);

  ParseShader(shader_binary_content);

  VkShaderModuleCreateInfo shader_module_ci{};
  {
    shader_module_ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_ci.codeSize = shader_binary_content.size();
    shader_module_ci.pCode = reinterpret_cast<const uint32_t *>(shader_binary_content.data());
  }

  VK_CHECK(vkCreateShaderModule(GraphicsContext::Get()->GetDevice(), &shader_module_ci, nullptr, &shader_module_));
}

ShaderModule::~ShaderModule() {
}

const VkShaderModule ShaderModule::GetShaderModule() const {
  return shader_module_;
}

VkShaderStageFlagBits ShaderModule::GetShaderStage() const {
  return shader_stage_;
}

std::span<const VkPushConstantRange> ShaderModule::GetPushConstantRanges() const {
  return push_constant_ranges_;
}

std::span<const VkVertexInputAttributeDescription> ShaderModule::GetVertexInputAttributes() const {
  return input_attribute_descriptions_;
}

std::span<const VkVertexInputBindingDescription> ShaderModule::GetVertexInputBinding() const {
  return input_binding_description_;
}

std::span<const std::vector<VkDescriptorSetLayoutBinding>> ShaderModule::GetDescriptorSetLayoutBindings() const {
  return descriptor_set_bindings_;
}

} // namespace Innsmouth