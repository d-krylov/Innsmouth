#ifndef INNSMOUTH_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_H
#define INNSMOUTH_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_H

#include "innsmouth/graphics/buffer/buffer.h"
#include "bottom_level_geometry.h"

namespace Innsmouth {

class BottomLevelAccelerationStructure {
public:
  BottomLevelAccelerationStructure() = default;

  BottomLevelAccelerationStructure(std::span<const BottomLevelGeometry> bottom_geometries);

  std::span<const VkAccelerationStructureKHR> GetAccelerationStructures() const;

  VkAccelerationStructureKHR GetAccelerationStructure(uint32_t index) const;

protected:
  void CreateAccelerationStructures(std::span<const BottomLevelGeometry> bottom_geometries);

private:
  std::vector<VkAccelerationStructureKHR> acceleration_structures_;
  Buffer acceleration_structures_buffer_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_H