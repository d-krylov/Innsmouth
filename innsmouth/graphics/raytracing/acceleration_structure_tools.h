#ifndef INNSMOUTH_ACCELERATION_STRUCTURE_TOOLS_H
#define INNSMOUTH_ACCELERATION_STRUCTURE_TOOLS_H

#include "bottom_level_geometry.h"

namespace Innsmouth {

struct InstanceData {
  Matrix4f transform_{1.0f};
};

struct BottomLevelAccelerationStructureInstances {
  std::vector<InstanceData> instances_;
  VkAccelerationStructureKHR acceleration_structure_{VK_NULL_HANDLE};
};

struct AccelerationSize {
  std::size_t total_acceleration_size = 0;
  std::size_t total_scratch_size = 0;
};

struct AccelerationInformation {
  std::size_t acceleration_offset_ = 0;
  std::size_t acceleration_size_ = 0;
  std::size_t scratch_offset_ = 0;
};

AccelerationStructureBuildSizesInfoKHR GetAccelerationStructureSize(const BottomLevelGeometry &bottom_geometry);
AccelerationSize GetAccelerationStructureSize(std::span<const BottomLevelGeometry> geometries, std::span<AccelerationInformation> out);
AccelerationStructureBuildSizesInfoKHR GetAccelerationStructureSize(uint32_t instances);
uint32_t GetTotalInstancesCount(std::span<const BottomLevelAccelerationStructureInstances> bottom_instances);

VkAccelerationStructureKHR CreateAccelerationStructure(VkBuffer acceleration_buffer, const AccelerationInformation &acceleration_information,
                                                       AccelerationStructureTypeKHR type);

AccelerationStructureBuildGeometryInfoKHR GetBuildGeometryInformation(std::span<const AccelerationStructureGeometryKHR> geometries,
                                                                      VkDeviceAddress scratch_buffer,
                                                                      VkAccelerationStructureKHR acceleration_structure,
                                                                      AccelerationStructureTypeKHR type);

} // namespace Innsmouth

#endif // INNSMOUTH_ACCELERATION_STRUCTURE_TOOLS_H