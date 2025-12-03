#include "bottom_level_geometry.h"
#include <algorithm>

namespace Innsmouth {

void BottomLevelGeometry::AddTriangleGeometry(const TriangleGeometrySpecification &specification) {
  AccelerationStructureGeometryKHR geometry;
  AccelerationStructureBuildRangeInfoKHR range;
  geometry.geometryType = GeometryTypeKHR::E_TRIANGLES_KHR;
  geometry.flags = GeometryMaskBitsKHR::E_OPAQUE_BIT_KHR;
  geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
  geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
  geometry.geometry.triangles.vertexData.deviceAddress = specification.vbo_offset_;
  geometry.geometry.triangles.maxVertex = specification.vertices_count_ - 1;
  geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
  geometry.geometry.triangles.indexData.deviceAddress = specification.ibo_offset_;
  geometry.geometry.triangles.vertexStride = specification.vertex_stride_;

  range.firstVertex = 0;
  range.primitiveOffset = 0;
  range.primitiveCount = specification.indices_count_ / 3;

  geometries_.emplace_back(geometry);
  ranges_.emplace_back(range);
}

void BottomLevelGeometry::AddBoundingBoxGeometry(VkDeviceAddress aabb_buffer) {
  AccelerationStructureGeometryKHR geometry;
  geometry.geometryType = GeometryTypeKHR::E_AABBS_KHR;
  geometry.flags = GeometryMaskBitsKHR::E_OPAQUE_BIT_KHR;
  geometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
  geometry.geometry.aabbs.data.deviceAddress = aabb_buffer;
  geometry.geometry.aabbs.stride = sizeof(AabbPositionsKHR);
  geometries_.emplace_back(geometry);
}

std::vector<uint32_t> BottomLevelGeometry::GetPrimitiveCounts() const {
  std::vector<uint32_t> counts;
  std::ranges::transform(ranges_, std::back_inserter(counts), std::identity(), &AccelerationStructureBuildRangeInfoKHR::primitiveCount);
  return counts;
}

std::span<const AccelerationStructureGeometryKHR> BottomLevelGeometry::GetGeometries() const {
  return geometries_;
}

std::span<const AccelerationStructureBuildRangeInfoKHR> BottomLevelGeometry::GetRanges() const {
  return ranges_;
}

std::size_t BottomLevelGeometry::GetGeometryCount() const {
  return geometries_.size();
}

} // namespace Innsmouth
