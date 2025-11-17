#include "acceleration_structure.h"

namespace Innsmouth {

std::size_t AlignUp(std::size_t value, std::size_t alignment) {
  return (value + alignment - 1) & ~(alignment - 1);
}

auto CreateAccelerationStructureGeometry() {
  AccelerationStructureGeometryKHR geometry;
  { geometry.geometryType = GeometryTypeKHR::E_TRIANGLES_KHR; }
}

AccelerationStructure::AccelerationStructure() {
}

} // namespace Innsmouth