#include "innsmouth/core/include/core.h"
#include "acceleration_structure.h"
#include "innsmouth/graphics/command/command_buffer.h"
#include <numeric>
#include <print>

namespace Innsmouth {

std::vector<VkAccelerationStructureKHR>
AccelerationStructure::BuildAccelerationStructures(VkBuffer main_buffer, VkDeviceAddress scratch_buffer,
                                                   std::span<const AccelerationInformation> acceleration_informations,
                                                   std::span<const BottomLevelGeometry> bottom_geometries) {

  auto bottom_geometries_count = bottom_geometries.size();

  std::vector<AccelerationStructureGeometryKHR> geometries(bottom_geometries_count);
  std::vector<AccelerationStructureBuildGeometryInfoKHR> geometry_infos(bottom_geometries_count);

  auto total_ranges = 0;
  for (const auto &bottom_geometry : bottom_geometries) {
    total_ranges += bottom_geometry.GetGeometryCount();
  }

  std::vector<AccelerationStructureBuildRangeInfoKHR> ranges;
  std::vector<const AccelerationStructureBuildRangeInfoKHR *> range_pointers;

  ranges.reserve(total_ranges);
  for (const auto bottom_geometry : bottom_geometries) {
    auto bottom_geometry_ranges = bottom_geometry.GetRanges();
    range_pointers.emplace_back(ranges.data() + ranges.size());
    ranges.insert(ranges.end(), bottom_geometry_ranges.begin(), bottom_geometry_ranges.end());
  }

  auto type = AccelerationStructureTypeKHR::E_BOTTOM_LEVEL_KHR;
  std::vector<VkAccelerationStructureKHR> acceleration_structures(bottom_geometries_count, VK_NULL_HANDLE);
  for (auto i = 0; i < bottom_geometries_count; i++) {
    auto &bottom_geometry = bottom_geometries[i];
    auto scratch_address = scratch_buffer + acceleration_informations[i].scratch_offset_;
    acceleration_structures[i] = CreateAccelerationStructure(main_buffer, acceleration_informations[i], type);
    geometry_infos[i] = GetBuildGeometryInformation(bottom_geometry.GetGeometries(), scratch_address, acceleration_structures[i], type);
  }

  CommandBuffer command_buffer(GraphicsContext::Get()->GetGraphicsQueueIndex());
  command_buffer.Begin();
  command_buffer.CommandBuildAccelerationStructure(geometry_infos, range_pointers);
  command_buffer.End();
  command_buffer.Submit();

  return acceleration_structures;
}

} // namespace Innsmouth