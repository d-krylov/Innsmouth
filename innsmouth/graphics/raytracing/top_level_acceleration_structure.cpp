#include "top_level_acceleration_structure.h"
#include "innsmouth/graphics/command/command_buffer.h"
#include <algorithm>
#include <ranges>
#include <print>

namespace Innsmouth {

TransformMatrixKHR ConvertTransform(const Matrix4f &matrix) {
  TransformMatrixKHR out{};

  out.matrix[0][0] = matrix[0][0];
  out.matrix[0][1] = matrix[0][1];
  out.matrix[0][2] = matrix[0][2];
  out.matrix[0][3] = matrix[0][3];

  out.matrix[1][0] = matrix[1][0];
  out.matrix[1][1] = matrix[1][1];
  out.matrix[1][2] = matrix[1][2];
  out.matrix[1][3] = matrix[1][3];

  out.matrix[2][0] = matrix[2][0];
  out.matrix[2][1] = matrix[2][1];
  out.matrix[2][2] = matrix[2][2];
  out.matrix[2][3] = matrix[2][3];

  return out;
}

auto GetBottomLevelAccelerationStructuresAddress(VkAccelerationStructureKHR acceleration_structure) {
  AccelerationStructureDeviceAddressInfoKHR device_address_info;
  device_address_info.accelerationStructure = acceleration_structure;
  auto address = vkGetAccelerationStructureDeviceAddressKHR(GraphicsContext::Get()->GetDevice(), device_address_info);
  return address;
}

auto GetInstanceVector(std::span<const BottomLevelAccelerationStructureInstances> bottom_instances) {
  std::vector<AccelerationStructureInstanceKHR> instance_vector;
  for (const auto &[bottom_instance_index, bottom_instance] : std::views::enumerate(bottom_instances)) {
    auto buffer_device_address = GetBottomLevelAccelerationStructuresAddress(bottom_instance.acceleration_structure_);
    for (const auto &[instance_index, instance] : std::views::enumerate(bottom_instance.instances_)) {
      AccelerationStructureInstanceKHR acceleration_structure_instance;
      acceleration_structure_instance.transform = ConvertTransform(instance.transform_);
      acceleration_structure_instance.instanceCustomIndex = 0;
      acceleration_structure_instance.mask = 0xff;
      acceleration_structure_instance.accelerationStructureReference = buffer_device_address;
      instance_vector.emplace_back(acceleration_structure_instance);
    }
  }
  return instance_vector;
}

void TopLevelAccelerationStructure::Build(std::span<const BottomLevelAccelerationStructureInstances> bottom_instances) {
  auto usage = BufferUsageMaskBits::E_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | BufferUsageMaskBits::E_SHADER_DEVICE_ADDRESS_BIT;
  auto size = sizeof(AccelerationStructureInstanceKHR) * bottom_instances.size();

  Buffer instance_buffer(size, usage, Buffer::CPU);

  auto instance_vector = GetInstanceVector(bottom_instances);
  instance_buffer.SetData<AccelerationStructureInstanceKHR>(instance_vector);

  AccelerationStructureGeometryKHR geometry;
  geometry.geometryType = GeometryTypeKHR::E_INSTANCES_KHR;
  geometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
  geometry.geometry.instances.data.deviceAddress = instance_buffer.GetBufferAddress();

  std::array<AccelerationStructureBuildGeometryInfoKHR, 1> build_geometry_infos;
  build_geometry_infos[0].type = AccelerationStructureTypeKHR::E_TOP_LEVEL_KHR;
  build_geometry_infos[0].flags = BuildAccelerationStructureMaskBitsKHR::E_PREFER_FAST_TRACE_BIT_KHR;
  build_geometry_infos[0].mode = BuildAccelerationStructureModeKHR::E_BUILD_KHR;
  build_geometry_infos[0].geometryCount = 1;
  build_geometry_infos[0].pGeometries = &geometry;

  uint32_t primitive_count = 0;
  for (const auto &bottom_instance : bottom_instances) {
    primitive_count += bottom_instance.instances_.size();
  }

  AccelerationStructureBuildSizesInfoKHR build_sizes_info;
  vkGetAccelerationStructureBuildSizesKHR(GraphicsContext::Get()->GetDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
                                          build_geometry_infos[0], &primitive_count, build_sizes_info);

  BufferUsageMask tlas_usage = BufferUsageMaskBits::E_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
  BufferUsageMask scratch_usage = BufferUsageMaskBits::E_STORAGE_BUFFER_BIT | BufferUsageMaskBits::E_SHADER_DEVICE_ADDRESS_BIT;
  Buffer scratch_buffer(build_sizes_info.buildScratchSize, scratch_usage, AllocationCreateMaskBits::E_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
  acceleration_structures_buffer_ = Buffer(build_sizes_info.accelerationStructureSize, tlas_usage, {});

  AccelerationStructureCreateInfoKHR acceleration_structure_ci;
  acceleration_structure_ci.buffer = acceleration_structures_buffer_.GetHandle();
  acceleration_structure_ci.size = build_sizes_info.accelerationStructureSize;
  acceleration_structure_ci.type = AccelerationStructureTypeKHR::E_TOP_LEVEL_KHR;

  VK_CHECK(vkCreateAccelerationStructureKHR(GraphicsContext::Get()->GetDevice(), acceleration_structure_ci, nullptr, &acceleration_structure_));

  build_geometry_infos[0].dstAccelerationStructure = acceleration_structure_;
  build_geometry_infos[0].scratchData.deviceAddress = scratch_buffer.GetBufferAddress();

  AccelerationStructureBuildRangeInfoKHR build_range;
  build_range.primitiveCount = primitive_count;
  std::array<const AccelerationStructureBuildRangeInfoKHR *, 1> build_range_pointers;
  build_range_pointers[0] = &build_range;

  CommandBuffer command_buffer(GraphicsContext::Get()->GetGraphicsQueueIndex());
  command_buffer.Begin();
  command_buffer.CommandBuildAccelerationStructure(build_geometry_infos, build_range_pointers);
  command_buffer.End();
  command_buffer.Submit();
}

TopLevelAccelerationStructure::TopLevelAccelerationStructure(std::span<const BottomLevelAccelerationStructureInstances> instances) {
  Build(instances);
}

VkAccelerationStructureKHR TopLevelAccelerationStructure::GetAccelerationStructure() const {
  return acceleration_structure_;
}

} // namespace Innsmouth