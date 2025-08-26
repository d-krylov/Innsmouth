#include "graphics_pipeline.h"
#include <print>
#include <ranges>
#include <algorithm>
#include <map>

namespace Innsmouth {

// clang-format off
std::vector<VkDynamicState> GetDynamicStates() {
  return std::vector<VkDynamicState>{
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
    VK_DYNAMIC_STATE_CULL_MODE,
    VK_DYNAMIC_STATE_FRONT_FACE,
    VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE,
    VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE
  };
}
// clang-format on

std::vector<VkDescriptorSetLayout> GraphicsPipeline::CreateDescriptorSetLayouts(std::span<const ShaderModule> shader_modules) {
  std::map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> all_descriptors;
  for (const auto &shader_module : shader_modules) {
    auto descriptor_set_bindings = shader_module.GetDescriptorSetLayoutBindings();
    for (const auto &[index, bindings] : std::views::enumerate(descriptor_set_bindings)) {
      all_descriptors[index].insert(all_descriptors[index].end(), bindings.begin(), bindings.end());
    }
  }

  std::vector<VkDescriptorSetLayout> descriptor_set_layouts(all_descriptors.size());

  for (auto &[set_number, set_bindings] : all_descriptors) {
    std::ranges::sort(set_bindings, [](const auto &a, const auto &b) { return a.binding < b.binding; });

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_ci{};
    {
      descriptor_set_layout_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptor_set_layout_ci.bindingCount = set_bindings.size();
      descriptor_set_layout_ci.pBindings = set_bindings.data();
      descriptor_set_layout_ci.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
    }

    VK_CHECK(
      vkCreateDescriptorSetLayout(GraphicsContext::Get()->GetDevice(), &descriptor_set_layout_ci, nullptr, &descriptor_set_layouts[set_number]));
  }

  return descriptor_set_layouts;
}

GraphicsPipeline::GraphicsPipeline(const std::filesystem::path &vertex_shader, const std::filesystem::path &fragment_shader,
                                   VkFormat color_format) {
  std::vector<ShaderModule> shader_modules{vertex_shader, fragment_shader};

  auto vertex_attributes = shader_modules[0].GetVertexInputAttributes();
  auto vertex_binding = shader_modules[0].GetVertexInputBinding();

  std::array color_formats = {color_format};

  CreateGraphicsPipeline(shader_modules, color_formats, vertex_attributes, vertex_binding);
}

GraphicsPipeline::~GraphicsPipeline() {
}

void GraphicsPipeline::CreateGraphicsPipeline(std::span<const ShaderModule> shader_modules, std::span<const VkFormat> color_formats,
                                              std::span<const VkVertexInputAttributeDescription> vertex_input_attributes,
                                              std::span<const VkVertexInputBindingDescription> vertex_input_bindings) {

  std::vector<VkPipelineShaderStageCreateInfo> shader_stages_cis;

  for (const auto &shader_module : shader_modules) {
    VkPipelineShaderStageCreateInfo shader_stage_ci{};
    {
      shader_stage_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      shader_stage_ci.stage = shader_module.GetShaderStage();
      shader_stage_ci.module = shader_module.GetShaderModule();
      shader_stage_ci.pName = "main";
    }
    shader_stages_cis.emplace_back(shader_stage_ci);
  }

  // VERTEX INPUT STATE
  VkPipelineVertexInputStateCreateInfo vertex_input_state_ci{};
  {
    vertex_input_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_ci.vertexBindingDescriptionCount = vertex_input_bindings.size();
    vertex_input_state_ci.pVertexBindingDescriptions = vertex_input_bindings.data();
    vertex_input_state_ci.vertexAttributeDescriptionCount = vertex_input_attributes.size();
    vertex_input_state_ci.pVertexAttributeDescriptions = vertex_input_attributes.data();
  }

  // INPUT ASSEMBLY STATE
  VkPipelineInputAssemblyStateCreateInfo input_assembly_state_ci{};
  {
    input_assembly_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_state_ci.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_state_ci.primitiveRestartEnable = VK_FALSE;
  }

  // VIEWPORT STATE
  VkPipelineViewportStateCreateInfo viewport_state_ci{};
  {
    viewport_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_ci.viewportCount = 1;
    viewport_state_ci.scissorCount = 1;
  }

  // RASTERIZATION STATE
  VkPipelineRasterizationStateCreateInfo rasterization_state_ci{};
  {
    rasterization_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state_ci.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_state_ci.lineWidth = 1.0f;
    rasterization_state_ci.depthBiasEnable = VK_FALSE;
  }

  // MULTISAMPLE STATE
  VkPipelineMultisampleStateCreateInfo multisample_state_ci{};
  {
    multisample_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state_ci.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  }

  // DEPTH STENCIL STATE
  VkPipelineDepthStencilStateCreateInfo depth_stencil_state_ci{};
  {
    depth_stencil_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_state_ci.depthCompareOp = VK_COMPARE_OP_LESS;
    depth_stencil_state_ci.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_state_ci.minDepthBounds = 0.0f;
    depth_stencil_state_ci.maxDepthBounds = 0.0f;
    depth_stencil_state_ci.stencilTestEnable = VK_FALSE;
    depth_stencil_state_ci.front = {};
    depth_stencil_state_ci.back = {};
  }

  // COLOR BLENDING STATE
  VkPipelineColorBlendAttachmentState color_blend_attachment_state{};
  {
    color_blend_attachment_state.blendEnable = VK_FALSE;
    color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment_state.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  }

  VkPipelineColorBlendStateCreateInfo color_blending_state_ci{};
  {
    color_blending_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending_state_ci.attachmentCount = 1;
    color_blending_state_ci.pAttachments = &color_blend_attachment_state;
  }

  // DYNAMIC STATES
  auto dynamic_states = GetDynamicStates();

  VkPipelineDynamicStateCreateInfo dynamic_state_ci{};
  {
    dynamic_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_ci.dynamicStateCount = dynamic_states.size();
    dynamic_state_ci.pDynamicStates = dynamic_states.data();
  }

  descriptor_set_layouts_ = CreateDescriptorSetLayouts(shader_modules);

  // PIPELINE LAYOUT
  VkPipelineLayoutCreateInfo pipeline_layout_ci{};
  {
    pipeline_layout_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_ci.setLayoutCount = descriptor_set_layouts_.size();
    pipeline_layout_ci.pSetLayouts = descriptor_set_layouts_.data();
    pipeline_layout_ci.pushConstantRangeCount = 0;
    pipeline_layout_ci.pPushConstantRanges = nullptr; // push_constant_ranges.data();
  }

  VK_CHECK(vkCreatePipelineLayout(GraphicsContext::Get()->GetDevice(), &pipeline_layout_ci, nullptr, &pipeline_layout_));

  // RENDERING CREATE INFO
  VkPipelineRenderingCreateInfo pipeline_rendering_ci{};
  {
    pipeline_rendering_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipeline_rendering_ci.colorAttachmentCount = color_formats.size();
    pipeline_rendering_ci.pColorAttachmentFormats = color_formats.data();
    pipeline_rendering_ci.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
    pipeline_rendering_ci.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
  }

  VkGraphicsPipelineCreateInfo graphics_pipeline_ci{};
  {
    graphics_pipeline_ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_ci.stageCount = shader_stages_cis.size();
    graphics_pipeline_ci.pStages = shader_stages_cis.data();
    graphics_pipeline_ci.pVertexInputState = &vertex_input_state_ci;
    graphics_pipeline_ci.pInputAssemblyState = &input_assembly_state_ci;
    graphics_pipeline_ci.pTessellationState = nullptr;
    graphics_pipeline_ci.pViewportState = &viewport_state_ci;
    graphics_pipeline_ci.pRasterizationState = &rasterization_state_ci;
    graphics_pipeline_ci.pMultisampleState = &multisample_state_ci;
    graphics_pipeline_ci.pDepthStencilState = &depth_stencil_state_ci;
    graphics_pipeline_ci.pColorBlendState = &color_blending_state_ci;
    graphics_pipeline_ci.pDynamicState = &dynamic_state_ci;
    graphics_pipeline_ci.layout = pipeline_layout_;
    graphics_pipeline_ci.pNext = &pipeline_rendering_ci;
  }

  VK_CHECK(vkCreateGraphicsPipelines(GraphicsContext::Get()->GetDevice(), nullptr, 1, &graphics_pipeline_ci, nullptr, &graphics_pipeline_));
}

} // namespace Innsmouth