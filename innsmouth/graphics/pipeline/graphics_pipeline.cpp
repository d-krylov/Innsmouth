#include "graphics_pipeline.h"
#include "graphics/include/graphics.h"
#include "graphics/include/structure_tools.h"
#include <ranges>
#include <unordered_map>
#include <algorithm>
#include <iostream>

namespace Innsmouth {

// clang-format off
std::vector<VkDynamicState> GetDynamicStates() {
  return std::vector{
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
    VK_DYNAMIC_STATE_CULL_MODE,
    VK_DYNAMIC_STATE_FRONT_FACE,
    VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE,
    VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE
  };
}
// clang-format on

GraphicsPipeline::GraphicsPipeline(const std::filesystem::path &vertex_shader, const std::filesystem::path &fragment_shader,
                                   Format color_format, const std::optional<VkPipelineColorBlendAttachmentState> &blend_attachment_state) {
  std::vector<ShaderModule> shader_modules{vertex_shader, fragment_shader};

  auto vertex_attributes = shader_modules[0].GetVertexInputAttributes();
  auto vertex_binding = shader_modules[0].GetVertexInputBinding();

  auto color_blend_attachment = blend_attachment_state.value_or(CreateColorBlendAttachmentState(false));

  CreateGraphicsPipeline(shader_modules, std::views::single(color_format), color_blend_attachment, vertex_attributes, vertex_binding);
}

GraphicsPipeline::~GraphicsPipeline() {}

std::vector<VkDescriptorSetLayout> CreateDescriptorSetLayouts(std::span<const ShaderModule> shader_modules) {
  std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> all_descriptors;
  for (const auto &shader_module : shader_modules) {
    auto descriptor_set_bindings = shader_module.GetDescriptorSetLayoutBindings();
    for (const auto &[i, bindings] : std::views::enumerate(descriptor_set_bindings)) {
      all_descriptors[i].insert(all_descriptors[i].end(), bindings.begin(), bindings.end());
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

    VK_CHECK(vkCreateDescriptorSetLayout(Device(), &descriptor_set_layout_ci, nullptr, &descriptor_set_layouts[set_number]));
  }

  return descriptor_set_layouts;
}

void GraphicsPipeline::CreateGraphicsPipeline(std::span<const ShaderModule> shader_modules, std::span<const Format> color_formats,
                                              const VkPipelineColorBlendAttachmentState &color_blend_attachment_state,
                                              std::span<const VkVertexInputAttributeDescription> vertex_input_attributes,
                                              std::span<const VkVertexInputBindingDescription> vertex_input_bindings) {
  auto total_push_constants = std::views::transform(shader_modules, [](auto &&x) { return x.GetPushConstantRanges(); });
  auto push_constant_ranges = std::ranges::to<std::vector>(total_push_constants | std::views::join);
  auto total_shader_stages = std::views::transform(shader_modules, [](auto &&x) {
    return VkPipelineShaderStageCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                           .stage = x.GetShaderStage(),
                                           .module = x.GetShaderModule(),
                                           .pName = "main"};
  });

  auto shader_stages_ci = std::ranges::to<std::vector>(total_shader_stages);

  descriptor_set_layouts_ = CreateDescriptorSetLayouts(shader_modules);

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

  // PIPELINE LAYOUT
  VkPipelineLayoutCreateInfo pipeline_layout_ci{};
  {
    pipeline_layout_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_ci.setLayoutCount = descriptor_set_layouts_.size();
    pipeline_layout_ci.pSetLayouts = descriptor_set_layouts_.data();
    pipeline_layout_ci.pPushConstantRanges = push_constant_ranges.data();
    pipeline_layout_ci.pushConstantRangeCount = push_constant_ranges.size();
  }

  VK_CHECK(vkCreatePipelineLayout(Device(), &pipeline_layout_ci, nullptr, &pipeline_layout_));

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

  auto vk_color_formats = std::bit_cast<std::span<const VkFormat>>(color_formats);

  VkPipelineRenderingCreateInfo pipeline_rendering_ci{};
  {
    pipeline_rendering_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipeline_rendering_ci.colorAttachmentCount = vk_color_formats.size();
    pipeline_rendering_ci.pColorAttachmentFormats = vk_color_formats.data();
    pipeline_rendering_ci.depthAttachmentFormat = VkFormat(Format::UNDEFINED);
    pipeline_rendering_ci.stencilAttachmentFormat = VkFormat(Format::UNDEFINED);
  }

  VkGraphicsPipelineCreateInfo graphics_pipeline_ci{};
  {
    graphics_pipeline_ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_ci.stageCount = shader_stages_ci.size();
    graphics_pipeline_ci.pStages = shader_stages_ci.data();
    graphics_pipeline_ci.pVertexInputState = &vertex_input_state_ci;
    graphics_pipeline_ci.pInputAssemblyState = &input_assembly_state_ci;
    // graphics_pipeline_ci.pTessellationState = nullptr;
    graphics_pipeline_ci.pViewportState = &viewport_state_ci;
    graphics_pipeline_ci.pRasterizationState = &rasterization_state_ci;
    graphics_pipeline_ci.pMultisampleState = &multisample_state_ci;
    graphics_pipeline_ci.pDepthStencilState = &depth_stencil_state_ci;
    graphics_pipeline_ci.pColorBlendState = &color_blending_state_ci;
    graphics_pipeline_ci.pDynamicState = &dynamic_state_ci;
    graphics_pipeline_ci.layout = pipeline_layout_;
    graphics_pipeline_ci.pNext = &pipeline_rendering_ci;
  }

  VK_CHECK(vkCreateGraphicsPipelines(Device(), nullptr, 1, &graphics_pipeline_ci, nullptr, &graphics_pipeline_));
}

} // namespace Innsmouth