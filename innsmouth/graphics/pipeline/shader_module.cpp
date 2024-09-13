#include "shader_module.h"
#include "innsmouth/core/include/tools.h"
#include "innsmouth/graphics/include/graphics.h"
#include "innsmouth/graphics/include/graphics_helpers.h"
#include "spirv_reflect/spirv_reflect.h"

namespace Innsmouth {

void ShaderModule::GetDescriptorSets(const SpvReflectShaderModule &module, VkShaderStageFlags stage) {
  auto descriptor_sets = Enumerate<SpvReflectDescriptorSet *>(spvReflectEnumerateDescriptorSets, &module);
  for (auto &set : descriptor_sets) {
    auto &bindings = descriptor_map_[set->set];
    for (uint32_t i = 0; i < set->binding_count; ++i) {
      const auto *rf_binding = set->bindings[i];
      auto &binding = bindings.emplace_back();
      {
        binding.binding = rf_binding->binding;
        binding.descriptorType = VkDescriptorType(rf_binding->descriptor_type);
        binding.descriptorCount = 1;
        binding.stageFlags = stage;
      }

      for (uint32_t k = 0; k < rf_binding->array.dims_count; ++k) {
        binding.descriptorCount *= rf_binding->array.dims[k];
      }
    }

    std::sort(bindings.begin(), bindings.end(),
              [](const auto &a, const auto &b) { return a.binding < b.binding; });
  }
}

[[nodiscard]] std::vector<VkVertexInputAttributeDescription>
GetShaderInputs(const SpvReflectShaderModule &module) {
  auto inputs = Enumerate<SpvReflectInterfaceVariable *>(spvReflectEnumerateInputVariables, &module);
  std::vector<VkVertexInputAttributeDescription> attributes;
  attributes.reserve(inputs.size());
  for (auto input : inputs) {
    if (input->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) {
      continue;
    }
    VkVertexInputAttributeDescription attribute{};
    {
      attribute.location = input->location;
      attribute.binding = 0;
      attribute.format = static_cast<VkFormat>(input->format);
      attribute.offset = 0;
    }

    attributes.push_back(attribute);
  }
  auto location_sort = [](const auto &a, const auto &b) { return a.location < b.location; };
  std::sort(std::begin(attributes), std::end(attributes), location_sort);
  return attributes;
}

[[nodiscard]] auto GetPushConstants(const SpvReflectShaderModule &module, VkShaderStageFlags stage) {
  auto reflect_blocks = Enumerate<SpvReflectBlockVariable *>(spvReflectEnumeratePushConstantBlocks, &module);
  std::vector<VkPushConstantRange> pc_ranges;
  for (auto block : reflect_blocks) {
    VkPushConstantRange range{};
    {
      range.offset = block->absolute_offset;
      range.size = block->size;
      range.stageFlags = stage;
    }
    pc_ranges.emplace_back(range);
  }
  return pc_ranges;
}

void ShaderModule::GetShaderInformation(const std::vector<std::byte> &data) {
  SpvReflectShaderModule module;

  auto result = spvReflectCreateShaderModule(data.size(), data.data(), &module);

  if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS) {
  }

  shader_stage_ = module.shader_stage;

  inputs_ = GetShaderInputs(module);

  binding_description_.binding = 0;
  binding_description_.stride = 0;
  binding_description_.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  for (auto &input : inputs_) {
    input.offset = binding_description_.stride;
    binding_description_.stride += vkuFormatElementSize(input.format);
  }

  push_constant_ranges_ = GetPushConstants(module, shader_stage_);

  GetDescriptorSets(module, shader_stage_);

  spvReflectDestroyShaderModule(&module);
}

ShaderModule::ShaderModule(const std::filesystem::path &path) {

  auto shader_bin = ReadBinaryFile(path);

  GetShaderInformation(shader_bin);

  VkShaderModuleCreateInfo shader_module_ci{};
  {
    shader_module_ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_ci.codeSize = shader_bin.size();
    shader_module_ci.pCode = reinterpret_cast<const uint32_t *>(shader_bin.data());
  }

  VK_CHECK(vkCreateShaderModule(Device(), &shader_module_ci, nullptr, &shader_module_));
}

ShaderModule::~ShaderModule() {}

} // namespace Innsmouth