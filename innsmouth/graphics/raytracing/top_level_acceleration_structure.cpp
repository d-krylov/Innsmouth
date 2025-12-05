#include "acceleration_structure.h"
#include "innsmouth/graphics/command/command_buffer.h"
#include <algorithm>
#include <ranges>
#include <print>

namespace Innsmouth {

TransformMatrixKHR ConvertTransform(const Matrix4f &matrix) {
  TransformMatrixKHR out{};

  out.matrix[0][0] = matrix[0][0];
  out.matrix[0][1] = matrix[1][0];
  out.matrix[0][2] = matrix[2][0];
  out.matrix[0][3] = matrix[3][0];
  out.matrix[1][0] = matrix[0][1];
  out.matrix[1][1] = matrix[1][1];
  out.matrix[1][2] = matrix[2][1];
  out.matrix[1][3] = matrix[3][1];
  out.matrix[2][0] = matrix[0][2];
  out.matrix[2][1] = matrix[1][2];
  out.matrix[2][2] = matrix[2][2];
  out.matrix[2][3] = matrix[3][2];

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
      acceleration_structure_instance.instanceCustomIndex = instance_index;
      acceleration_structure_instance.mask = 0xff;
      acceleration_structure_instance.accelerationStructureReference = buffer_device_address;
      instance_vector.emplace_back(acceleration_structure_instance);
    }
  }
  return instance_vector;
}

VkAccelerationStructureKHR
AccelerationStructure::BuildAccelerationStructures(VkBuffer main_buffer, VkDeviceAddress scratch,
                                                   const AccelerationInformation &acceleration_information,
                                                   std::span<const BottomLevelAccelerationStructureInstances> bottom_instances) {
  uint32_t primitive_count = GetTotalInstancesCount(bottom_instances);

  auto usage = BufferUsageMaskBits::E_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | BufferUsageMaskBits::E_SHADER_DEVICE_ADDRESS_BIT;
  auto size = sizeof(AccelerationStructureInstanceKHR) * primitive_count;
  Buffer instance_buffer(size, usage, Buffer::CPU);

  auto instance_vector = GetInstanceVector(bottom_instances);
  instance_buffer.SetData<AccelerationStructureInstanceKHR>(instance_vector);

  std::array<AccelerationStructureGeometryKHR, 1> geometries;
  geometries[0].geometryType = GeometryTypeKHR::E_INSTANCES_KHR;
  geometries[0].geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
  geometries[0].geometry.instances.data.deviceAddress = instance_buffer.GetBufferAddress();

  std::array<AccelerationStructureBuildGeometryInfoKHR, 1> geometry_bi;

  VkAccelerationStructureKHR acceleration_structure = VK_NULL_HANDLE;

  acceleration_structure = CreateAccelerationStructure(main_buffer, acceleration_information, AccelerationStructureTypeKHR::E_TOP_LEVEL_KHR);
  geometry_bi[0] = GetBuildGeometryInformation(geometries, scratch, acceleration_structure, AccelerationStructureTypeKHR::E_TOP_LEVEL_KHR);

  std::array<AccelerationStructureBuildRangeInfoKHR, 1> build_ranges;
  std::array<const AccelerationStructureBuildRangeInfoKHR *, 1> build_range_pointers;

  build_ranges[0].primitiveCount = primitive_count;
  build_range_pointers[0] = build_ranges.data();

  CommandBuffer command_buffer(GraphicsContext::Get()->GetGraphicsQueueIndex());
  command_buffer.Begin();
  command_buffer.CommandBuildAccelerationStructure(geometry_bi, build_range_pointers);
  command_buffer.End();
  command_buffer.Submit();

  return acceleration_structure;
}

} // namespace Innsmouth