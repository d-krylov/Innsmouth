#ifndef INNSMOUTH_TOP_LEVEL_ACCELERATION_STRUCTURE_H
#define INNSMOUTH_TOP_LEVEL_ACCELERATION_STRUCTURE_H

#include "innsmouth/graphics/buffer/buffer.h"
#include "innsmouth/mathematics/include/mathematics_types.h"

namespace Innsmouth {

struct InstanceData {
  Matrix4f transform_{1.0f};
};

struct BottomLevelAccelerationStructureInstances {
  std::vector<InstanceData> instances_;
  VkAccelerationStructureKHR acceleration_structure_{VK_NULL_HANDLE};
};

class TopLevelAccelerationStructure {
public:
  TopLevelAccelerationStructure() = default;

  TopLevelAccelerationStructure(std::span<const BottomLevelAccelerationStructureInstances> instances);

  VkAccelerationStructureKHR GetAccelerationStructure() const;

  void Build(std::span<const BottomLevelAccelerationStructureInstances> instances);

private:
  VkAccelerationStructureKHR acceleration_structure_{VK_NULL_HANDLE};
  Buffer acceleration_structures_buffer_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_TOP_LEVEL_ACCELERATION_STRUCTURE_H