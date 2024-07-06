#include "graphics_pipeline.h"
#include "easyloggingpp/easylogging++.h"
#include "graphics/include/graphics.h"
#include "shader_module.h"

namespace Innsmouth {

void GraphicsPipeline::ProcessDescriptorSets(const std::vector<ShaderModule> &modules) {
  std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> merged_descriptor;
  for (const auto &shader : modules) {
    const auto &shader_descriptor_map = shader.GetDescriptorSetData();
    for (const auto &[set_number, bindings] : shader_descriptor_map) {
      auto &merged_bindings = merged_descriptor[set_number];
      merged_bindings.insert(merged_bindings.end(), bindings.begin(), bindings.end());
    }
  }
  descriptor_set_layouts_.resize(merged_descriptor.size());
  for (auto &[set_number, bindings] : merged_descriptor) {
    std::sort(bindings.begin(), bindings.end(),
              [](const auto &a, const auto &b) { return a.binding < b.binding; });

    for (const auto &binding : bindings) {
      LOG(INFO) << "Set: " << set_number << ", "
                << "Binding: " << binding.binding << ", "
                << string_VkDescriptorType(binding.descriptorType) << ", "
                << "Descriptor count: " << binding.descriptorCount;
    }

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_ci{};
    {
      descriptor_set_layout_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptor_set_layout_ci.bindingCount = bindings.size();
      descriptor_set_layout_ci.pBindings = bindings.data();
      descriptor_set_layout_ci.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
    }

    VK_CHECK(vkCreateDescriptorSetLayout(Device(), &descriptor_set_layout_ci, nullptr,
                                         &descriptor_set_layouts_[set_number]));
  }
}

GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineDescription &description) {
  std::vector<ShaderModule> shader_modules(description.paths_.begin(), description.paths_.end());

  std::vector<VkPipelineShaderStageCreateInfo> shader_stages_ci;
  std::vector<VkPushConstantRange> push_constant_ranges;

  shader_stages_ci.reserve(shader_modules.size());

  for (const auto &shader : shader_modules) {
    VkPipelineShaderStageCreateInfo shader_stage_ci{};
    {
      shader_stage_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      shader_stage_ci.stage = static_cast<VkShaderStageFlagBits>(shader.GetStage());
      shader_stage_ci.module = shader;
      shader_stage_ci.pName = "main";
    }
    shader_stages_ci.emplace_back(shader_stage_ci);
    const auto &pc_range = shader.GetPushConstantRanges();
    push_constant_ranges.insert(push_constant_ranges.end(), pc_range.begin(), pc_range.end());
    VkDescriptorSetLayout descriptor_set_layout;
    const auto &descriptor_set_data = shader.GetDescriptorSetData();
  }

  ProcessDescriptorSets(shader_modules);

  std::vector<VkVertexInputAttributeDescription> attributes;
  std::vector<VkVertexInputBindingDescription> bindings;

  if (description.vertex_attributes_.empty()) {
    attributes = shader_modules[0].GetVertexInputAttributes();
  } else {
    attributes = description.vertex_attributes_;
  }

  if (description.vertex_bindings_.empty()) {
    bindings.emplace_back(shader_modules[0].GetVertexInputBinding());
  } else {
    bindings = description.vertex_bindings_;
  }

  VkPipelineVertexInputStateCreateInfo vertex_input_state_ci{};
  {
    vertex_input_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_ci.vertexBindingDescriptionCount = bindings.size();
    vertex_input_state_ci.pVertexBindingDescriptions = bindings.data();
    vertex_input_state_ci.vertexAttributeDescriptionCount = attributes.size();
    vertex_input_state_ci.pVertexAttributeDescriptions = attributes.data();
  }

  VkPipelineInputAssemblyStateCreateInfo input_assembly_state_ci{};
  {
    input_assembly_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_state_ci.topology = description.topology_;
    input_assembly_state_ci.primitiveRestartEnable = VK_FALSE;
  }

  std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT,
                                                VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamic_state_ci{};
  {
    dynamic_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_ci.dynamicStateCount = dynamic_states.size();
    dynamic_state_ci.pDynamicStates = dynamic_states.data();
  }

  VkPipelineRasterizationStateCreateInfo rasterization_state_ci{};
  {
    rasterization_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state_ci.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_state_ci.lineWidth = 1.0f;
    rasterization_state_ci.cullMode = VK_CULL_MODE_NONE;
    rasterization_state_ci.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_state_ci.depthBiasEnable = VK_FALSE;
  }

  VkPipelineLayoutCreateInfo pipeline_layout_ci{};
  {
    pipeline_layout_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_ci.setLayoutCount = descriptor_set_layouts_.size();
    pipeline_layout_ci.pSetLayouts = descriptor_set_layouts_.data();
    pipeline_layout_ci.pPushConstantRanges = push_constant_ranges.data();
    pipeline_layout_ci.pushConstantRangeCount = push_constant_ranges.size();
  }

  VK_CHECK(vkCreatePipelineLayout(Device(), &pipeline_layout_ci, nullptr, &pipeline_layout_));

  VkPipelineViewportStateCreateInfo viewport_state_ci{};
  {
    viewport_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_ci.viewportCount = 1;
    viewport_state_ci.scissorCount = 1;
  }

  VkPipelineMultisampleStateCreateInfo multisample_state_ci{};
  {
    multisample_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state_ci.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  }

  VkPipelineColorBlendAttachmentState blend_attachment_state{};
  {
    blend_attachment_state.blendEnable = VK_TRUE;
    blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
    blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
    blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  }

  VkPipelineColorBlendStateCreateInfo color_blending_state_ci{};
  {
    color_blending_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending_state_ci.attachmentCount = 1;
    color_blending_state_ci.pAttachments = &blend_attachment_state;
  }

  VkPipelineDepthStencilStateCreateInfo depth_stencil_state_ci{};
  {
    depth_stencil_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_state_ci.depthTestEnable = VK_FALSE;
    depth_stencil_state_ci.depthWriteEnable = VK_FALSE;
    depth_stencil_state_ci.depthCompareOp = VK_COMPARE_OP_NEVER;
    depth_stencil_state_ci.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_state_ci.minDepthBounds = 0.0f;
    depth_stencil_state_ci.maxDepthBounds = 0.0f;
    depth_stencil_state_ci.stencilTestEnable = VK_FALSE;
    depth_stencil_state_ci.front = {};
    depth_stencil_state_ci.back = {};
  }

  VkPipelineRenderingCreateInfo pipeline_rendering_ci{};
  {
    pipeline_rendering_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipeline_rendering_ci.colorAttachmentCount = description.color_formats_.size();
    pipeline_rendering_ci.pColorAttachmentFormats = description.color_formats_.data();
  }

  VkGraphicsPipelineCreateInfo graphics_pipeline_ci{};
  {
    graphics_pipeline_ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_ci.stageCount = shader_stages_ci.size();
    graphics_pipeline_ci.pStages = shader_stages_ci.data();
    graphics_pipeline_ci.pVertexInputState = &vertex_input_state_ci;
    graphics_pipeline_ci.pInputAssemblyState = &input_assembly_state_ci;
    graphics_pipeline_ci.pViewportState = &viewport_state_ci;
    graphics_pipeline_ci.pRasterizationState = &rasterization_state_ci;
    graphics_pipeline_ci.pMultisampleState = &multisample_state_ci;
    graphics_pipeline_ci.pColorBlendState = &color_blending_state_ci;
    graphics_pipeline_ci.pDynamicState = &dynamic_state_ci;
    graphics_pipeline_ci.layout = pipeline_layout_;
    graphics_pipeline_ci.subpass = 0;
    graphics_pipeline_ci.pDepthStencilState = &depth_stencil_state_ci;
    graphics_pipeline_ci.basePipelineHandle = VK_NULL_HANDLE;
    graphics_pipeline_ci.pNext = &pipeline_rendering_ci;
    graphics_pipeline_ci.renderPass = VK_NULL_HANDLE;
  }

  VK_CHECK(vkCreateGraphicsPipelines(Device(), VK_NULL_HANDLE, 1, &graphics_pipeline_ci, nullptr,
                                     &graphics_pipeline_));
}

GraphicsPipeline::~GraphicsPipeline() {}

} // namespace Innsmouth