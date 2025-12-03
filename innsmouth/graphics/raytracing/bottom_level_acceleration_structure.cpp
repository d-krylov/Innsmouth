#include "innsmouth/core/include/core.h"
#include "bottom_level_acceleration_structure.h"
#include "innsmouth/graphics/command/command_buffer.h"
#include <numeric>
#include <print>

namespace Innsmouth {

struct AccelerationInformation {
  std::size_t acceleration_offset_;
  std::size_t acceleration_size_;
  std::size_t scratch_offset_;
};

struct AccelerationSize {
  std::size_t total_acceleration_size = 0;
  std::size_t total_scratch_size = 0;
};

auto GetAccelerationStructureInforamtion(std::span<const BottomLevelGeometry> bottom_geometries,
                                         std::span<AccelerationStructureBuildGeometryInfoKHR> build_geometry_infos,
                                         std::span<AccelerationInformation> acceleration_information) {

  std::size_t total_acceleration_size = 0, total_scratch_size = 0;
  auto bottom_geometries_count = bottom_geometries.size();
  auto alignment = 256;

  for (auto i = 0; i < bottom_geometries_count; i++) {
    auto geometries = bottom_geometries[i].GetGeometries();
    build_geometry_infos[i].flags = BuildAccelerationStructureMaskBitsKHR::E_PREFER_FAST_TRACE_BIT_KHR;
    build_geometry_infos[i].mode = BuildAccelerationStructureModeKHR::E_BUILD_KHR;
    build_geometry_infos[i].type = AccelerationStructureTypeKHR::E_BOTTOM_LEVEL_KHR;
    build_geometry_infos[i].geometryCount = geometries.size();
    build_geometry_infos[i].pGeometries = geometries.data();

    auto primitive_counts = bottom_geometries[i].GetPrimitiveCounts();

    AccelerationStructureBuildSizesInfoKHR build_sizes_info;
    vkGetAccelerationStructureBuildSizesKHR(GraphicsContext::Get()->GetDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
                                            build_geometry_infos[i], primitive_counts.data(), build_sizes_info);

    acceleration_information[i].acceleration_offset_ = total_acceleration_size;
    acceleration_information[i].acceleration_size_ = build_sizes_info.accelerationStructureSize;
    acceleration_information[i].scratch_offset_ = total_scratch_size;

    total_acceleration_size = AlignUp(total_acceleration_size + build_sizes_info.accelerationStructureSize, alignment);
    total_scratch_size = AlignUp(total_scratch_size + build_sizes_info.buildScratchSize, alignment);
  }

  return AccelerationSize(total_acceleration_size, total_scratch_size);
}

void BottomLevelAccelerationStructure::CreateAccelerationStructures(std::span<const BottomLevelGeometry> bottom_geometries) {

  auto bottom_geometries_count = bottom_geometries.size();

  std::vector<AccelerationStructureGeometryKHR> geometries(bottom_geometries_count);
  std::vector<AccelerationStructureBuildGeometryInfoKHR> geometry_infos(bottom_geometries_count);
  std::vector<AccelerationInformation> acceleration_information(bottom_geometries_count);

  auto acceleration_size = GetAccelerationStructureInforamtion(bottom_geometries, geometry_infos, acceleration_information);

  BufferUsageMask blas_usage = BufferUsageMaskBits::E_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | BufferUsageMaskBits::E_SHADER_DEVICE_ADDRESS_BIT;
  BufferUsageMask scratch_usage = BufferUsageMaskBits::E_SHADER_DEVICE_ADDRESS_BIT | BufferUsageMaskBits::E_STORAGE_BUFFER_BIT;
  Buffer scratch_buffer(acceleration_size.total_scratch_size, scratch_usage, AllocationCreateMaskBits::E_DEDICATED_MEMORY_BIT);
  acceleration_structures_buffer_ =
    Buffer(acceleration_size.total_acceleration_size, blas_usage, AllocationCreateMaskBits::E_DEDICATED_MEMORY_BIT);

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

  acceleration_structures_.resize(bottom_geometries_count, VK_NULL_HANDLE);

  auto scratch_address = scratch_buffer.GetBufferAddress();
  auto buffer_handle = acceleration_structures_buffer_.GetHandle();

  for (auto i = 0; i < bottom_geometries_count; i++) {
    AccelerationStructureCreateInfoKHR acceleration_structure_ci;

    acceleration_structure_ci.buffer = acceleration_structures_buffer_.GetHandle();
    acceleration_structure_ci.offset = acceleration_information[i].acceleration_offset_;
    acceleration_structure_ci.size = acceleration_information[i].acceleration_size_;
    acceleration_structure_ci.type = AccelerationStructureTypeKHR::E_BOTTOM_LEVEL_KHR;

    VK_CHECK(
      vkCreateAccelerationStructureKHR(GraphicsContext::Get()->GetDevice(), acceleration_structure_ci, nullptr, &acceleration_structures_[i]));

    geometry_infos[i].dstAccelerationStructure = acceleration_structures_[i];
    geometry_infos[i].scratchData.deviceAddress = scratch_address + acceleration_information[i].scratch_offset_;
  }

  CommandBuffer command_buffer(GraphicsContext::Get()->GetGraphicsQueueIndex());
  command_buffer.Begin();
  command_buffer.CommandBuildAccelerationStructure(geometry_infos, range_pointers);
  command_buffer.End();
  command_buffer.Submit();
}

BottomLevelAccelerationStructure::BottomLevelAccelerationStructure(std::span<const BottomLevelGeometry> bottom_geometries) {
  CreateAccelerationStructures(bottom_geometries);
}

std::span<const VkAccelerationStructureKHR> BottomLevelAccelerationStructure::GetAccelerationStructures() const {
  return acceleration_structures_;
}

VkAccelerationStructureKHR BottomLevelAccelerationStructure::GetAccelerationStructure(uint32_t index) const {
  return acceleration_structures_[index];
}

} // namespace Innsmouth