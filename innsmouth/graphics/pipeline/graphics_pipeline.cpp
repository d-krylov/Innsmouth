#include "graphics_pipeline.h"
#include "innsmouth/graphics/core/structure_tools.h"
#include "innsmouth/graphics/core/graphics_formats.h"
#include <print>
#include <ranges>
#include <algorithm>
#include <map>

namespace Innsmouth {

VkPipelineLayout CreatePipelineLayout(std::span<const VkDescriptorSetLayout> set_layouts, std::span<const PushConstantRange> push_constants) {
  VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
  PipelineLayoutCreateInfo pipeline_layout_ci;
  {
    pipeline_layout_ci.setLayoutCount = set_layouts.size();
    pipeline_layout_ci.pSetLayouts = set_layouts.data();
    pipeline_layout_ci.pushConstantRangeCount = push_constants.size();
    pipeline_layout_ci.pPushConstantRanges = push_constants.data();
  }
  VK_CHECK(vkCreatePipelineLayout(GraphicsContext::Get()->GetDevice(), pipeline_layout_ci, nullptr, &pipeline_layout));
  return pipeline_layout;
}

VkDescriptorSetLayout CreateDescriptorSetLayout(std::span<const DescriptorSetLayoutBinding> descriptor_set_bindings, bool bindless) {
  VkDescriptorSetLayout descriptor_set_layout = VK_NULL_HANDLE;
  DescriptorSetLayoutCreateInfo descriptor_set_layout_ci;
  descriptor_set_layout_ci.bindingCount = descriptor_set_bindings.size();
  descriptor_set_layout_ci.pBindings = descriptor_set_bindings.data();

  DescriptorBindingMask binding_mask = DescriptorBindingMaskBits::E_UPDATE_AFTER_BIND_BIT | //
                                       DescriptorBindingMaskBits::E_PARTIALLY_BOUND_BIT |   //
                                       DescriptorBindingMaskBits::E_VARIABLE_DESCRIPTOR_COUNT_BIT;

  DescriptorSetLayoutBindingFlagsCreateInfo set_binding_flags_ci;
  set_binding_flags_ci.bindingCount = 1;
  set_binding_flags_ci.pBindingFlags = &binding_mask;

  if (bindless) {
    descriptor_set_layout_ci.flags = DescriptorSetLayoutCreateMaskBits::E_UPDATE_AFTER_BIND_POOL_BIT;
    descriptor_set_layout_ci.pNext = &set_binding_flags_ci;
  } else {
    descriptor_set_layout_ci.flags = DescriptorSetLayoutCreateMaskBits::E_PUSH_DESCRIPTOR_BIT;
  }

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

GraphicsPipeline::~GraphicsPipeline() {
}

VkPipeline CreateGraphicsPipeline(const PipelineSpecification &specification, std::span<const ShaderModule> shader_modules,
                                  VkPipelineLayout pipeline_layout) {
  std::vector<ShaderModuleCreateInfo> shader_modules_cis(shader_modules.size());
  std::vector<PipelineShaderStageCreateInfo> shader_stages_cis(shader_modules.size());

  for (auto i = 0; i < shader_modules.size(); i++) {
    ShaderModuleCreateInfo shader_module_ci;
    {
      shader_modules_cis[i].codeSize = shader_modules[i].GetSize();
      shader_modules_cis[i].pCode = shader_modules[i].GetBinaryData().data();
    }

    PipelineShaderStageCreateInfo shader_stage_ci;
    {
      shader_stages_cis[i].stage = shader_modules[i].GetShaderStage();
      shader_stages_cis[i].pNext = &shader_modules_cis[i];
      shader_stages_cis[i].pName = "main";
    }
  }

  // VERTEX INPUT STATE
  auto vertex_input_attributes = shader_modules[0].GetVertexInputAttributes();

  VertexInputBindingDescription vertex_binding_description;
  PipelineVertexInputStateCreateInfo vertex_input_state_ci;

  if (vertex_input_attributes.empty() == false) {

    const auto &back_attribute = vertex_input_attributes.back();

    auto stride = back_attribute.offset + GetFormatTexelBlockSize(back_attribute.format);

    vertex_binding_description.binding = 0;
    vertex_binding_description.inputRate = VertexInputRate::E_VERTEX;
    vertex_binding_description.stride = stride;

    vertex_input_state_ci.vertexBindingDescriptionCount = 1;
    vertex_input_state_ci.pVertexBindingDescriptions = &vertex_binding_description;
    vertex_input_state_ci.vertexAttributeDescriptionCount = vertex_input_attributes.size();
    vertex_input_state_ci.pVertexAttributeDescriptions = vertex_input_attributes.data();
  }

  // INPUT ASSEMBLY STATE
  PipelineInputAssemblyStateCreateInfo input_assembly_state_ci;
  input_assembly_state_ci.topology = PrimitiveTopology::E_TRIANGLE_LIST;

  // VIEWPORT STATE
  PipelineViewportStateCreateInfo viewport_state_ci;
  viewport_state_ci.viewportCount = 1;
  viewport_state_ci.scissorCount = 1;

  // RASTERIZATION STATE
  PipelineRasterizationStateCreateInfo rasterization_state_ci;
  rasterization_state_ci.polygonMode = PolygonMode::E_FILL;
  rasterization_state_ci.lineWidth = 1.0f;
  rasterization_state_ci.cullMode = CullModeMaskBits::E_BACK_BIT;
  rasterization_state_ci.frontFace = FrontFace::E_COUNTER_CLOCKWISE;

  // DYNAMIC STATES
  PipelineDynamicStateCreateInfo dynamic_state_ci;
  dynamic_state_ci.dynamicStateCount = specification.dynamic_states_.size();
  dynamic_state_ci.pDynamicStates = specification.dynamic_states_.data();

  // MULTISAMPLE STATE
  PipelineMultisampleStateCreateInfo multisample_state_ci;
  multisample_state_ci.rasterizationSamples = SampleCountMaskBits::E_1_BIT;

  // DEPTH STENCIL STATE
  PipelineDepthStencilStateCreateInfo depth_stencil_state_ci;
  depth_stencil_state_ci.depthCompareOp = CompareOp::E_LESS;
  depth_stencil_state_ci.depthTestEnable = false;
  depth_stencil_state_ci.stencilTestEnable = false;

  // COLOR BLENDING STATE
  auto color_blend_attachment_state = GetColorBlendAttachmentState(true);

  PipelineColorBlendStateCreateInfo color_blending_state_ci;
  color_blending_state_ci.attachmentCount = 1;
  color_blending_state_ci.pAttachments = &color_blend_attachment_state;

  // RENDERING CREATE INFO
  PipelineRenderingCreateInfo pipeline_rendering_ci;

  pipeline_rendering_ci.colorAttachmentCount = specification.color_formats_.size();
  pipeline_rendering_ci.pColorAttachmentFormats = specification.color_formats_.data();
  pipeline_rendering_ci.depthAttachmentFormat = specification.depth_format_;
  pipeline_rendering_ci.stencilAttachmentFormat = specification.stencil_format_;

  GraphicsPipelineCreateInfo graphics_pipeline_ci;

  graphics_pipeline_ci.stageCount = shader_stages_cis.size();
  graphics_pipeline_ci.pStages = shader_stages_cis.data();
  graphics_pipeline_ci.pVertexInputState = &vertex_input_state_ci;
  graphics_pipeline_ci.pInputAssemblyState = &input_assembly_state_ci;
  graphics_pipeline_ci.pViewportState = &viewport_state_ci;
  graphics_pipeline_ci.pRasterizationState = &rasterization_state_ci;
  graphics_pipeline_ci.pMultisampleState = &multisample_state_ci;
  graphics_pipeline_ci.pDepthStencilState = &depth_stencil_state_ci;
  graphics_pipeline_ci.pColorBlendState = &color_blending_state_ci;
  graphics_pipeline_ci.pDynamicState = &dynamic_state_ci;
  graphics_pipeline_ci.layout = pipeline_layout;
  graphics_pipeline_ci.pNext = &pipeline_rendering_ci;

  VkPipeline graphics_pipeline = VK_NULL_HANDLE;
  VK_CHECK(vkCreateGraphicsPipelines(GraphicsContext::Get()->GetDevice(), nullptr, 1, graphics_pipeline_ci, nullptr, &graphics_pipeline));
  return graphics_pipeline;
}

VkPipelineLayout GraphicsPipeline::GetPipelineLayout() const {
  return pipeline_layout_;
}

VkPipeline GraphicsPipeline::GetPipeline() const {
  return graphics_pipeline_;
}

std::span<const VkDescriptorSetLayout> GraphicsPipeline::GetDescriptorSetLayouts() const {
  return descriptor_set_layouts_;
}

GraphicsPipeline::GraphicsPipeline(const PipelineSpecification &pipeline_specification) {
  std::vector<ShaderModule> shader_modules;
  DescriptorSetLayoutBindingMap push_descriptor_sets;
  DescriptorSetLayoutBindingMap pool_descriptor_sets;
  for (const auto &shader_path : pipeline_specification.shader_paths_) {
    auto &shader_module = shader_modules.emplace_back(shader_path);
    MergeDescriptorSets(push_descriptor_sets, shader_module.GetPushDescriptorSetLayoutBindings());
    MergeDescriptorSets(pool_descriptor_sets, shader_module.GetPoolDescriptorSetLayoutBindings());
  }
  descriptor_set_layouts_.resize(push_descriptor_sets.size() + pool_descriptor_sets.size());

  for (const auto &[set, bindings] : push_descriptor_sets) {
    descriptor_set_layouts_[set] = CreateDescriptorSetLayout(bindings, false);
  }

  for (const auto &[set, bindings] : pool_descriptor_sets) {
    descriptor_set_layouts_[set] = CreateDescriptorSetLayout(bindings, true);
  }

  pipeline_layout_ = CreatePipelineLayout(descriptor_set_layouts_, shader_modules[0].GetPushConstantRanges());
  graphics_pipeline_ = CreateGraphicsPipeline(pipeline_specification, shader_modules, pipeline_layout_);
}

GraphicsPipeline::GraphicsPipeline(GraphicsPipeline &&other) noexcept {
  graphics_pipeline_ = std::exchange(other.graphics_pipeline_, VK_NULL_HANDLE);
  pipeline_layout_ = std::exchange(other.pipeline_layout_, VK_NULL_HANDLE);
  descriptor_set_layouts_ = std::move(other.descriptor_set_layouts_);
}

GraphicsPipeline &GraphicsPipeline::operator=(GraphicsPipeline &&other) noexcept {
  std::swap(graphics_pipeline_, other.graphics_pipeline_);
  std::swap(pipeline_layout_, other.pipeline_layout_);
  std::swap(descriptor_set_layouts_, other.descriptor_set_layouts_);
  return *this;
}

} // namespace Innsmouth