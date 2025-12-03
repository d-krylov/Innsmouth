#ifndef INNSMOUTH_BOTTOM_LEVEL_GEOMETRY_H
#define INNSMOUTH_BOTTOM_LEVEL_GEOMETRY_H

#include "innsmouth/graphics/core/graphics_types.h"
#include "innsmouth/mathematics/include/mathematics_types.h"
#include <vector>

namespace Innsmouth {

struct TriangleGeometrySpecification {
  VkDeviceAddress vbo_offset_ = 0;
  VkDeviceAddress ibo_offset_ = 0;
  std::size_t vertices_count_ = 0;
  std::size_t indices_count_ = 0;
  std::size_t vertex_stride_ = 0;
};

class BottomLevelGeometry {
public:
  void AddTriangleGeometry(const TriangleGeometrySpecification &specification);

  void AddBoundingBoxGeometry(VkDeviceAddress aabb_buffer);

  std::span<const AccelerationStructureGeometryKHR> GetGeometries() const;
  std::span<const AccelerationStructureBuildRangeInfoKHR> GetRanges() const;

  std::vector<uint32_t> GetPrimitiveCounts() const;

  std::size_t GetGeometryCount() const;

private:
  std::vector<AccelerationStructureGeometryKHR> geometries_;
  std::vector<AccelerationStructureBuildRangeInfoKHR> ranges_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_BOTTOM_LEVEL_GEOMETRY_H