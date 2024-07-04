#include "shader_module.h"
#include "core/include/tools.h"
#include "easyloggingpp/easylogging++.h"
#include "graphics/include/graphics.h"
#include "spirv_reflect/spirv_reflect.h"

namespace Innsmouth {

void SPV_VERIFY(SpvReflectResult result) {
  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    LOG(FATAL) << result;
  }
}

[[nodiscard]] std::vector<SpvReflectInterfaceVariable *>
EnumerateInputVariables(const SpvReflectShaderModule &module) {
  uint32_t input_count{0};
  SPV_VERIFY(spvReflectEnumerateInputVariables(&module, &input_count, nullptr));
  std::vector<SpvReflectInterfaceVariable *> inputs(input_count, nullptr);
  SPV_VERIFY(spvReflectEnumerateInputVariables(&module, &input_count, inputs.data()));
  return inputs;
}

[[nodiscard]] std::vector<SpvReflectBlockVariable *>
EnumeratePushConstants(const SpvReflectShaderModule &module) {
  uint32_t count{0};
  SPV_VERIFY(spvReflectEnumeratePushConstantBlocks(&module, &count, nullptr));
  std::vector<SpvReflectBlockVariable *> pc(count, nullptr);
  SPV_VERIFY(spvReflectEnumeratePushConstantBlocks(&module, &count, pc.data()));
  return pc;
}

[[nodiscard]] std::vector<SpvReflectDescriptorSet *>
EnumerateDescriptorSets(const SpvReflectShaderModule &module) {
  uint32_t descriptor_set_count{0};
  spvReflectEnumerateDescriptorSets(&module, &descriptor_set_count, nullptr);
  std::vector<SpvReflectDescriptorSet *> descriptor_sets(descriptor_set_count, nullptr);
  spvReflectEnumerateDescriptorSets(&module, &descriptor_set_count, descriptor_sets.data());
  return descriptor_sets;
}

void ShaderModule::GetDescriptorSets(const SpvReflectShaderModule &module,
                                     VkShaderStageFlags stage) {
  auto descriptor_sets = EnumerateDescriptorSets(module);
  for (auto &set : descriptor_sets) {
    auto &bindings = descriptor_map_[set->set];
    for (uint32_t i = 0; i < set->binding_count; ++i) {
      const auto *rf_binding = set->bindings[i];
      auto &binding = bindings.emplace_back();
      {
        binding.binding = rf_binding->binding;
        binding.descriptorType = static_cast<VkDescriptorType>(rf_binding->descriptor_type);
        binding.descriptorCount = 1;
        binding.stageFlags = stage;
      }

      for (uint32_t k = 0; k < rf_binding->array.dims_count; ++k) {
        binding.descriptorCount *= rf_binding->array.dims[k];
      }

      LOG(INFO) << "Set: " << set->set << ", "
                << "Binding: " << binding.binding << ", "
                << string_VkDescriptorType(binding.descriptorType) << ", "
                << "Descriptor count: " << binding.descriptorCount;
    }

    std::sort(bindings.begin(), bindings.end(),
              [](const auto &a, const auto &b) { return a.binding < b.binding; });
  }
}

[[nodiscard]] std::vector<VkVertexInputAttributeDescription>
GetShaderInputs(const SpvReflectShaderModule &module) {
  auto inputs = EnumerateInputVariables(module);
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

[[nodiscard]] std::vector<VkPushConstantRange>
GetPushConstants(const SpvReflectShaderModule &module, VkShaderStageFlags stage) {
  auto reflect_blocks = EnumeratePushConstants(module);
  std::vector<VkPushConstantRange> pc_ranges;
  for (auto block : reflect_blocks) {
    VkPushConstantRange range{};
    {
      range.offset = block->absolute_offset;
      range.size = block->size;
      range.stageFlags = stage;
    }
    pc_ranges.emplace_back(range);
    LOG(INFO) << "Offset: " << range.offset << ", Size: " << range.size;
  }
  return pc_ranges;
}

void ShaderModule::GetShaderInformation(const std::vector<std::byte> &data) {
  SpvReflectShaderModule module;
  auto result = spvReflectCreateShaderModule(data.size(), data.data(), &module);

  if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS) {
    LOG(FATAL) << "Can't parse this data";
  }

  shader_stage_ = module.shader_stage;

  inputs_ = GetShaderInputs(module);

  binding_description_.binding = 0;
  binding_description_.stride = 0;
  binding_description_.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  for (auto &input : inputs_) {
    uint32_t format_size = FormatElementSize(input.format);
    input.offset = binding_description_.stride;
    binding_description_.stride += format_size;

    LOG(INFO) << "Format: " << string_VkFormat(input.format) << ", "
              << "Location: " << input.location << ", "
              << "Size: " << FormatElementSize(input.format);
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