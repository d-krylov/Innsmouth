#include "acceleration_structure_tools.h"
#include "innsmouth/core/include/core.h"

namespace Innsmouth {

VkAccelerationStructureKHR CreateAccelerationStructure(VkBuffer main_buffer, const AccelerationInformation &information,
                                                       AccelerationStructureTypeKHR type) {
  AccelerationStructureCreateInfoKHR acceleration_structure_ci;
  acceleration_structure_ci.buffer = main_buffer;
  acceleration_structure_ci.offset = information.acceleration_offset_;
  acceleration_structure_ci.size = information.acceleration_size_;
  acceleration_structure_ci.type = type;
  VkAccelerationStructureKHR acceleration_structure = VK_NULL_HANDLE;
  VK_CHECK(vkCreateAccelerationStructureKHR(GraphicsContext::Get()->GetDevice(), acceleration_structure_ci, nullptr, &acceleration_structure));
  return acceleration_structure;
}

AccelerationStructureBuildGeometryInfoKHR GetBuildGeometryInformation(std::span<const AccelerationStructureGeometryKHR> geometries,
                                                                      VkDeviceAddress scratch_buffer,
                                                                      VkAccelerationStructureKHR acceleration_structure,
                                                                      AccelerationStructureTypeKHR type) {
  AccelerationStructureBuildGeometryInfoKHR geometry_bi;
  geometry_bi.flags = BuildAccelerationStructureMaskBitsKHR::E_PREFER_FAST_TRACE_BIT_KHR;
  geometry_bi.mode = BuildAccelerationStructureModeKHR::E_BUILD_KHR;
  geometry_bi.type = type;
  geometry_bi.geometryCount = geometries.size();
  geometry_bi.pGeometries = geometries.data();
  geometry_bi.dstAccelerationStructure = acceleration_structure;
  geometry_bi.scratchData.deviceAddress = scratch_buffer;
  return geometry_bi;
}

AccelerationStructureBuildSizesInfoKHR GetAccelerationStructureSize(const BottomLevelGeometry &bottom_geometry) {
  auto device = GraphicsContext::Get()->GetDevice();
  auto geometries = bottom_geometry.GetGeometries();
  AccelerationStructureBuildGeometryInfoKHR geometry_bi;
  geometry_bi.flags = BuildAccelerationStructureMaskBitsKHR::E_PREFER_FAST_TRACE_BIT_KHR;
  geometry_bi.type = AccelerationStructureTypeKHR::E_BOTTOM_LEVEL_KHR;
  geometry_bi.geometryCount = geometries.size();
  geometry_bi.pGeometries = geometries.data();
  auto primitives = bottom_geometry.GetPrimitiveCounts();
  AccelerationStructureBuildSizesInfoKHR out_size;
  vkGetAccelerationStructureBuildSizesKHR(device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, geometry_bi, primitives.data(), out_size);
  return out_size;
}

AccelerationStructureBuildSizesInfoKHR GetAccelerationStructureSize(uint32_t instances) {
  auto device = GraphicsContext::Get()->GetDevice();
  AccelerationStructureBuildSizesInfoKHR out_size;
  AccelerationStructureGeometryKHR geometry;
  geometry.geometryType = GeometryTypeKHR::E_INSTANCES_KHR;
  geometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
  AccelerationStructureBuildGeometryInfoKHR geometry_bi;
  geometry_bi.flags = BuildAccelerationStructureMaskBitsKHR::E_PREFER_FAST_TRACE_BIT_KHR;
  geometry_bi.type = AccelerationStructureTypeKHR::E_TOP_LEVEL_KHR;
  geometry_bi.geometryCount = 1;
  geometry_bi.pGeometries = &geometry;
  vkGetAccelerationStructureBuildSizesKHR(device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, geometry_bi, &instances, out_size);
  return out_size;
}

AccelerationSize GetAccelerationStructureSize(std::span<const BottomLevelGeometry> bottom_geometries,
                                              std::span<AccelerationInformation> out_information) {
  auto alignment = 256;
  std::size_t total_acceleration_size = 0, total_scratch_size = 0;
  for (auto i = 0; i < bottom_geometries.size(); i++) {
    auto build_sizes_info = GetAccelerationStructureSize(bottom_geometries[i]);
    out_information[i].acceleration_offset_ = total_acceleration_size;
    out_information[i].acceleration_size_ = build_sizes_info.accelerationStructureSize;
    out_information[i].scratch_offset_ = total_scratch_size;
    total_acceleration_size = AlignUp(total_acceleration_size + build_sizes_info.accelerationStructureSize, alignment);
    total_scratch_size = AlignUp(total_scratch_size + build_sizes_info.buildScratchSize, alignment);
  }
  return AccelerationSize(total_acceleration_size, total_scratch_size);
}

uint32_t GetTotalInstancesCount(std::span<const BottomLevelAccelerationStructureInstances> bottom_instances) {
  uint32_t primitive_count = 0;
  for (const auto &bottom_instance : bottom_instances) {
    primitive_count += bottom_instance.instances_.size();
  }
  return primitive_count;
}

} // namespace Innsmouth