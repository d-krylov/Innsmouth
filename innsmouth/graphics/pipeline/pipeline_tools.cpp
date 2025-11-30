#include "pipeline_tools.h"

namespace Innsmouth {

VkPipelineLayout CreatePipelineLayout(std::span<const VkDescriptorSetLayout> set_layouts, std::span<const PushConstantRange> push_constants) {
  VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
  PipelineLayoutCreateInfo pipeline_layout_ci;
  pipeline_layout_ci.setLayoutCount = set_layouts.size();
  pipeline_layout_ci.pSetLayouts = set_layouts.data();
  pipeline_layout_ci.pushConstantRangeCount = push_constants.size();
  pipeline_layout_ci.pPushConstantRanges = push_constants.data();
  VK_CHECK(vkCreatePipelineLayout(GraphicsContext::Get()->GetDevice(), pipeline_layout_ci, nullptr, &pipeline_layout));
  return pipeline_layout;
}

VkDescriptorSetLayout CreateDescriptorSetLayout(std::span<const DescriptorSetLayoutBinding> descriptor_set_bindings,
                                                DescriptorSetLayoutCreateMask descriptor_set_create_mask) {

  DescriptorBindingMask binding_mask = DescriptorBindingMaskBits::E_UPDATE_AFTER_BIND_BIT | //
                                       DescriptorBindingMaskBits::E_PARTIALLY_BOUND_BIT |   //
                                       DescriptorBindingMaskBits::E_VARIABLE_DESCRIPTOR_COUNT_BIT;

  DescriptorSetLayoutBindingFlagsCreateInfo set_binding_flags_ci;
  set_binding_flags_ci.bindingCount = 1;
  set_binding_flags_ci.pBindingFlags = &binding_mask;

  DescriptorSetLayoutCreateMask bindless = DescriptorSetLayoutCreateMaskBits::E_UPDATE_AFTER_BIND_POOL_BIT;

  DescriptorSetLayoutCreateInfo descriptor_set_layout_ci;
  descriptor_set_layout_ci.bindingCount = descriptor_set_bindings.size();
  descriptor_set_layout_ci.pBindings = descriptor_set_bindings.data();
  descriptor_set_layout_ci.flags = descriptor_set_create_mask;
  descriptor_set_layout_ci.pNext = descriptor_set_create_mask.HasBits(bindless) ? &set_binding_flags_ci : nullptr;

  VkDescriptorSetLayout descriptor_set_layout = VK_NULL_HANDLE;
  VK_CHECK(vkCreateDescriptorSetLayout(GraphicsContext::Get()->GetDevice(), descriptor_set_layout_ci, 0, &descriptor_set_layout));
  return descriptor_set_layout;
}

void MergeDescriptorSets(DescriptorSetLayoutBindingMap &destination, const DescriptorSetLayoutBindingMap &source) {
  for (const auto &[set, set_bindings] : source) {
    auto &destination_bindings = destination[set];
    for (auto &set_binding : set_bindings) {
      auto it = std::ranges::find(destination_bindings, set_binding.binding, &VkDescriptorSetLayoutBinding::binding);
      if (it == destination_bindings.end()) {
        destination_bindings.emplace_back(set_binding);
      } else {
        it->stageFlags |= set_binding.stageFlags;
      }
    }
  }
}

std::vector<VkDescriptorSetLayout> CreateDescriptorSetLayouts(std::span<const ShaderModule> shader_modules) {
  std::vector<VkDescriptorSetLayout> descriptor_sets;
  DescriptorSetLayoutBindingMap push_descriptor_sets;
  DescriptorSetLayoutBindingMap pool_descriptor_sets;
  for (const auto &shader_module : shader_modules) {
    MergeDescriptorSets(push_descriptor_sets, shader_module.GetPushDescriptorSetLayoutBindings());
    MergeDescriptorSets(pool_descriptor_sets, shader_module.GetPoolDescriptorSetLayoutBindings());
  }

  descriptor_sets.resize(push_descriptor_sets.size() + pool_descriptor_sets.size());

  for (const auto &[set, bindings] : push_descriptor_sets) {
    descriptor_sets[set] = CreateDescriptorSetLayout(bindings, DescriptorSetLayoutCreateMaskBits::E_PUSH_DESCRIPTOR_BIT);
  }

  for (const auto &[set, bindings] : pool_descriptor_sets) {
    descriptor_sets[set] = CreateDescriptorSetLayout(bindings, DescriptorSetLayoutCreateMaskBits::E_UPDATE_AFTER_BIND_POOL_BIT);
  }

  return descriptor_sets;
}

} // namespace Innsmouth