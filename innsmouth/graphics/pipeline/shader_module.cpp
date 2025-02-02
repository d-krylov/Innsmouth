#include "shader_module.h"
#include "spirv_reflect.h"
#include "graphics/include/graphics.h"
#include "graphics/include/graphics_tools.h"
#include <ranges>
#include <algorithm>
#include <fstream>
#include <print>

void Print(const VkPushConstantRange &v) { std::print("Push Constant: offset - {0}, size - {1}\n", v.offset, v.size); }

void Print(const VkVertexInputAttributeDescription &v) {
  std::print("Vertex Input: binding - {0}, offset - {1}, location - {2}\n", v.binding, v.offset, v.location);
}

void Print(const VkDescriptorSetLayoutBinding &v) {
  std::print("Descriptor Set Layout Binding : binding - {0}, descriptor count - {1}\n", v.binding, v.descriptorCount);
}

namespace Innsmouth {

std::vector<std::byte> ReadBinaryFile(const std::filesystem::path &path) {
  auto size = std::filesystem::file_size(path);
  std::vector<std::byte> buffer(size);
  std::ifstream input_file(path.string(), std::ios_base::binary);
  input_file.read(reinterpret_cast<char *>(buffer.data()), size);
  return buffer;
}

std::vector<VkVertexInputAttributeDescription> GetShaderInputs(const SpvReflectShaderModule &module) {
  auto inputs = Enumerate(spvReflectEnumerateInputVariables, &module);
  auto is_not_builtin = [](const auto *input) { return (input->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) == 0; };
  auto to_attribute = [&](const auto &input) { return VkVertexInputAttributeDescription(input->location, 0, VkFormat(input->format), 0); };
  auto ret = inputs | std::views::filter(is_not_builtin) | std::views::transform(to_attribute) | std::ranges::to<std::vector>();
  std::ranges::sort(ret, [](auto &&a, auto &&b) { return a.location < b.location; });
  std::ranges::for_each(ret, [offset = 0](auto &input) mutable { input.offset = offset, offset += vkuFormatElementSize(input.format); });
  return ret;
}

std::vector<VkPushConstantRange> GetPushConstants(const SpvReflectShaderModule &module, VkShaderStageFlags stage) {
  auto reflect_blocks = Enumerate(spvReflectEnumeratePushConstantBlocks, &module);
  auto to_push_constants = [&](auto &&block) { return VkPushConstantRange(stage, block->absolute_offset, block->size); };
  return std::views::transform(reflect_blocks, to_push_constants) | std::ranges::to<std::vector>();
}

auto GetDescriptorSetBindings(const SpvReflectShaderModule &module, VkShaderStageFlags stage) {
  auto spv_descriptor_sets = Enumerate(spvReflectEnumerateDescriptorSets, &module);
  std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptor_bindings(spv_descriptor_sets.size());
  for (auto &&[index, spv_descriptor_set] : std::views::enumerate(spv_descriptor_sets)) {
    std::span<SpvReflectDescriptorBinding *> spv_set_bindings(spv_descriptor_set->bindings, spv_descriptor_set->binding_count);
    auto result_bingings = std::views::transform(spv_set_bindings, [&](auto &&spv_binding) {
      auto count = std::ranges::fold_left(std::views::take(spv_binding->array.dims, spv_binding->array.dims_count), 1, std::multiplies<>());
      return VkDescriptorSetLayoutBinding{spv_binding->binding, static_cast<VkDescriptorType>(spv_binding->descriptor_type), count, stage};
    });
    descriptor_bindings[index] = std::ranges::to<std::vector>(result_bingings);
  }
  return descriptor_bindings;
}

void ShaderModule::ParseShader(std::span<const std::byte> data) {
  SpvReflectShaderModule spv_module;
  auto status = spvReflectCreateShaderModule(data.size(), data.data(), &spv_module);
  shader_stage_ = static_cast<VkShaderStageFlagBits>(spv_module.shader_stage);
  input_attribute_descriptions_ = GetShaderInputs(spv_module);
  push_constant_ranges_ = GetPushConstants(spv_module, shader_stage_);
  descriptor_set_bindings_ = GetDescriptorSetBindings(spv_module, shader_stage_);

  auto offsets = std::views::transform(input_attribute_descriptions_, [](auto &attribute) { return attribute.offset; });
  auto stride = std::ranges::fold_left(offsets, 0, std::plus<>());

  if (input_attribute_descriptions_.size() > 0) {
    input_binding_description_.emplace_back(0, stride, VK_VERTEX_INPUT_RATE_VERTEX);
  }
}

ShaderModule::ShaderModule(const std::filesystem::path &path) {
  auto shader_bin = ReadBinaryFile(path);

  ParseShader(shader_bin);

  VkShaderModuleCreateInfo shader_module_ci{};
  {
    shader_module_ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_ci.codeSize = shader_bin.size();
    shader_module_ci.pCode = reinterpret_cast<const uint32_t *>(shader_bin.data());
  }

  VK_CHECK(vkCreateShaderModule(Device(), &shader_module_ci, nullptr, &shader_module_));
}

ShaderModule::~ShaderModule() {
  // vkDestroyShaderModule(Device(), shader_module_, nullptr);
}

} // namespace Innsmouth