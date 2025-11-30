#include "shader_binding_table.h"
#include "innsmouth/core/include/core.h"
#include "innsmouth/graphics/command/command_buffer.h"
#include <print>

namespace Innsmouth {

PhysicalDeviceRayTracingPipelinePropertiesKHR GetRayTracingPipelineProperties() {
  PhysicalDeviceRayTracingPipelinePropertiesKHR ray_tracing_pipeline_properties;
  PhysicalDeviceProperties2 physical_device_properties;
  physical_device_properties.pNext = &ray_tracing_pipeline_properties;
  vkGetPhysicalDeviceProperties2(GraphicsContext::Get()->GetPhysicalDevice(), physical_device_properties);
  return ray_tracing_pipeline_properties;
}

auto GetShaderBindingTableSize(const ShaderGroupSpecification &shader_groups, uint32_t aligned_handle_size, uint32_t group_alignment) {
  auto raygen_region_size = AlignUp(shader_groups.raygen_ * aligned_handle_size, group_alignment);
  auto miss_region_size = AlignUp(shader_groups.miss_ * aligned_handle_size, group_alignment);
  auto hit_region_size = AlignUp(shader_groups.hit_ * aligned_handle_size, group_alignment);
  auto callable_region_size = AlignUp(shader_groups.callable_ * aligned_handle_size, group_alignment);
  return raygen_region_size + miss_region_size + hit_region_size + callable_region_size;
}

void ShaderBindingTable::SetTables(const ShaderGroupSpecification &shader_groups, uint32_t aligned_handle_size, uint32_t group_alignment) {
  auto sbt_address = sbt_buffer_.GetBufferAddress();

  raygen_shader_binding_table_.size = aligned_handle_size; // AlignUp(shader_groups.raygen_ * aligned_handle_size, group_alignment);
  raygen_shader_binding_table_.stride = aligned_handle_size;
  raygen_shader_binding_table_.deviceAddress = sbt_address;

  miss_shader_binding_table_.size = AlignUp(shader_groups.miss_ * aligned_handle_size, group_alignment);
  miss_shader_binding_table_.stride = aligned_handle_size;
  miss_shader_binding_table_.deviceAddress = sbt_address + AlignUp(shader_groups.raygen_ * aligned_handle_size, group_alignment);

  hit_shader_binding_table_.size = AlignUp(shader_groups.hit_ * aligned_handle_size, group_alignment);
  hit_shader_binding_table_.stride = aligned_handle_size;
  hit_shader_binding_table_.deviceAddress = miss_shader_binding_table_.deviceAddress + miss_shader_binding_table_.size;

  callable_shader_binding_table_.size = AlignUp(shader_groups.callable_ * aligned_handle_size, group_alignment);
  callable_shader_binding_table_.stride = aligned_handle_size;
  callable_shader_binding_table_.deviceAddress = hit_shader_binding_table_.deviceAddress + hit_shader_binding_table_.size;
}

void CopyRegion(std::span<const std::byte> source, Buffer &destination, uint32_t count, uint32_t &offset, uint32_t &index, uint32_t handle_size,
                uint32_t aligned_handle_size, uint32_t base_alignment) {
  for (auto i = 0; i < count; i++) {
    destination.SetData<std::byte>(source.subspan(index * handle_size, handle_size), offset);
    offset += aligned_handle_size;
    index++;
  }
  offset = AlignUp(offset, base_alignment);
}

void ShaderBindingTable::CreateShaderBindingTable(VkPipeline pipeline, const ShaderGroupSpecification &shader_groups) {
  auto ray_tracing_properties = GetRayTracingPipelineProperties();
  auto handle_size = ray_tracing_properties.shaderGroupHandleSize;
  auto group_count = shader_groups.raygen_ + shader_groups.miss_ + shader_groups.hit_ + shader_groups.callable_;

  std::vector<std::byte> handles(group_count * handle_size);
  VK_CHECK(vkGetRayTracingShaderGroupHandlesKHR(GraphicsContext::Get()->GetDevice(), pipeline, 0, group_count, handles.size(), handles.data()));

  auto base_alignment = ray_tracing_properties.shaderGroupBaseAlignment;
  auto aligned_handle_size = AlignUp(handle_size, ray_tracing_properties.shaderGroupHandleAlignment);
  auto aligned_buffer_size = GetShaderBindingTableSize(shader_groups, aligned_handle_size, base_alignment);

  BufferUsageMask sbt_usage = BufferUsageMaskBits::E_SHADER_BINDING_TABLE_BIT_KHR | BufferUsageMaskBits::E_TRANSFER_DST_BIT |
                              BufferUsageMaskBits::E_SHADER_DEVICE_ADDRESS_BIT;

  Buffer scratch_buffer(aligned_buffer_size, BufferUsageMaskBits::E_TRANSFER_SRC_BIT, Buffer::CPU);
  sbt_buffer_ = Buffer(aligned_buffer_size, sbt_usage, AllocationCreateMaskBits::E_DEDICATED_MEMORY_BIT);

  SetTables(shader_groups, aligned_handle_size, ray_tracing_properties.shaderGroupBaseAlignment);

  uint32_t offset = 0;
  uint32_t index = 0;

  CopyRegion(handles, scratch_buffer, shader_groups.raygen_, offset, index, handle_size, aligned_handle_size, base_alignment);
  CopyRegion(handles, scratch_buffer, shader_groups.miss_, offset, index, handle_size, aligned_handle_size, base_alignment);
  CopyRegion(handles, scratch_buffer, shader_groups.hit_, offset, index, handle_size, aligned_handle_size, base_alignment);
  CopyRegion(handles, scratch_buffer, shader_groups.callable_, offset, index, handle_size, aligned_handle_size, base_alignment);

  CommandBuffer command_buffer(GraphicsContext::Get()->GetGraphicsQueueIndex());
  command_buffer.Begin();
  command_buffer.CommandCopyBuffer(scratch_buffer.GetHandle(), sbt_buffer_.GetHandle(), 0, 0, scratch_buffer.GetSize());
  command_buffer.End();
  command_buffer.Submit();
}

ShaderBindingTable::ShaderBindingTable(VkPipeline pipeline, const ShaderGroupSpecification &shader_groups) {
  CreateShaderBindingTable(pipeline, shader_groups);
}

} // namespace Innsmouth