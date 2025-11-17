#include "shader_module.h"
#include <algorithm>
#include <numeric>
#include <print>
#include <fstream>
#include <vulkan/utility/vk_format_utils.h>
#include <SPIRV-Reflect/spirv_reflect.h>

namespace Innsmouth {

std::vector<uint32_t> ReadBinaryFile(const std::filesystem::path &path) {
  auto size = std::filesystem::file_size(path);
  std::vector<uint32_t> buffer(size / sizeof(uint32_t));
  std::ifstream input_file(path.string(), std::ios_base::binary);
  input_file.read(reinterpret_cast<char *>(buffer.data()), size);
  return buffer;
}

std::vector<VertexInputAttributeDescription> ReflecttShaderInputs(const SpvReflectShaderModule &module) {
  auto offset = 0;
  auto inputs = Enumerate(spvReflectEnumerateInputVariables, &module);
  auto result = std::vector<VertexInputAttributeDescription>();
  std::ranges::sort(inputs, std::less(), &SpvReflectInterfaceVariable::location);
  for (const auto &input : inputs) {
    if ((input->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) != 0) continue;
    auto &vertex_input_description = result.emplace_back();
    vertex_input_description.location = input->location;
    vertex_input_description.binding = 0;
    vertex_input_description.format = static_cast<Format>(input->format);
    vertex_input_description.offset = offset;
    offset += vkuFormatTexelBlockSize(VkFormat(input->format));
  }
  return result;
}

auto ReflectDescriptorSetBindings(const SpvReflectShaderModule &module, ShaderStageMaskBits stage,
                                  DescriptorSetLayoutBindingMap &out_push_bindngs, DescriptorSetLayoutBindingMap &out_pull_bindngs) {
  auto spv_descriptor_sets = Enumerate(spvReflectEnumerateDescriptorSets, &module);
  std::map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> out_sets_bindings;
  for (const auto &spv_descriptor_set : spv_descriptor_sets) {
    std::span<SpvReflectDescriptorBinding *> spv_set_bindings(spv_descriptor_set->bindings, spv_descriptor_set->binding_count);
    for (const auto &spv_binding : spv_set_bindings) {
      std::span<uint32_t> dimensions(spv_binding->array.dims, spv_binding->array.dims_count);
      DescriptorSetLayoutBinding descriptor_set_binding;
      descriptor_set_binding.binding = spv_binding->binding;
      descriptor_set_binding.descriptorType = static_cast<DescriptorType>(spv_binding->descriptor_type);
      descriptor_set_binding.stageFlags = stage;
      if (spv_binding->type_description->op == SpvOpTypeRuntimeArray) {
        descriptor_set_binding.descriptorCount = 65536;
        out_pull_bindngs[spv_descriptor_set->set].emplace_back(descriptor_set_binding);
      } else {
        auto descriptor_count = std::accumulate(dimensions.begin(), dimensions.end(), 1u, std::multiplies<>());
        descriptor_set_binding.descriptorCount = descriptor_count;
        out_push_bindngs[spv_descriptor_set->set].emplace_back(descriptor_set_binding);
      }
    }
  }
}

std::vector<PushConstantRange> ReflectPushConstants(const SpvReflectShaderModule &module, ShaderStageMaskBits stage) {
  auto spv_push_constants = Enumerate(spvReflectEnumeratePushConstantBlocks, &module);
  auto push_constants = std::vector<PushConstantRange>();
  for (const auto &spv_block : spv_push_constants) {
    auto &push_constant_range = push_constants.emplace_back();
    push_constant_range.stageFlags = stage;
    push_constant_range.offset = spv_block->absolute_offset;
    push_constant_range.size = spv_block->size;
  }
  return push_constants;
}

void ShaderModule::ParseShader(std::span<const uint32_t> shader_binary_data) {
  SpvReflectShaderModule spv_module;
  auto status = spvReflectCreateShaderModule(shader_binary_data.size_bytes(), spirv_.data(), &spv_module);
  shader_stage_ = static_cast<ShaderStageMaskBits>(spv_module.shader_stage);
  push_constant_ranges_ = ReflectPushConstants(spv_module, shader_stage_);
  ReflectDescriptorSetBindings(spv_module, shader_stage_, push_descriptor_set_bindings_, pull_descriptor_set_bindings_);
  input_attribute_descriptions_ = ReflecttShaderInputs(spv_module);
}

ShaderModule::ShaderModule(const std::filesystem::path &shader_path) {
  spirv_ = ReadBinaryFile(shader_path);
  ParseShader(spirv_);
}

std::span<const uint32_t> ShaderModule::GetBinaryData() const {
  return spirv_;
}

ShaderStageMaskBits ShaderModule::GetShaderStage() const {
  return shader_stage_;
}

std::span<const PushConstantRange> ShaderModule::GetPushConstantRanges() const {
  return push_constant_ranges_;
}

std::span<const VertexInputAttributeDescription> ShaderModule::GetVertexInputAttributes() const {
  return input_attribute_descriptions_;
}

const DescriptorSetLayoutBindingMap &ShaderModule::GetPushDescriptorSetLayoutBindings() const {
  return push_descriptor_set_bindings_;
}

const DescriptorSetLayoutBindingMap &ShaderModule::GetPullDescriptorSetLayoutBindings() const {
  return pull_descriptor_set_bindings_;
}

std::size_t ShaderModule::GetSize() const {
  return GetBinaryData().size_bytes();
}

} // namespace Innsmouth