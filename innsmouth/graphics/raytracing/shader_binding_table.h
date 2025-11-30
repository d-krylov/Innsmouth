#ifndef INNSMOUTH_SHADER_BINDING_TABLE_H
#define INNSMOUTH_SHADER_BINDING_TABLE_H

#include "innsmouth/graphics/buffer/buffer.h"

namespace Innsmouth {

struct ShaderGroupSpecification {
  uint32_t raygen_ = 0;
  uint32_t miss_ = 0;
  uint32_t hit_ = 0;
  uint32_t callable_ = 0;
};

class ShaderBindingTable {
public:
  ShaderBindingTable() = default;

  ShaderBindingTable(VkPipeline pipeline, const ShaderGroupSpecification &shader_groups);

protected:
  void CreateShaderBindingTable(VkPipeline pipeline, const ShaderGroupSpecification &shader_groups);

  void SetTables(const ShaderGroupSpecification &shader_groups, uint32_t aligned_handle_size, uint32_t group_alignment);

public:
  Buffer sbt_buffer_;
  StridedDeviceAddressRegionKHR raygen_shader_binding_table_;
  StridedDeviceAddressRegionKHR miss_shader_binding_table_;
  StridedDeviceAddressRegionKHR hit_shader_binding_table_;
  StridedDeviceAddressRegionKHR callable_shader_binding_table_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_SHADER_BINDING_TABLE_H