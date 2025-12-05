#ifndef INNSMOUTH_ACCELERATION_STRUCTURE_H
#define INNSMOUTH_ACCELERATION_STRUCTURE_H

#include "innsmouth/graphics/buffer/buffer.h"
#include "acceleration_structure_tools.h"

namespace Innsmouth {

class AccelerationStructure {
public:
  AccelerationStructure() = default;

  ~AccelerationStructure();

  AccelerationStructure(const BottomLevelGeometry &bottom_geometry);
  AccelerationStructure(std::span<const BottomLevelAccelerationStructureInstances> bottom_instances);

  AccelerationStructure(const AccelerationStructure &) = delete;
  AccelerationStructure &operator=(const AccelerationStructure &) = delete;

  AccelerationStructure(AccelerationStructure &&other) noexcept;
  AccelerationStructure &operator=(AccelerationStructure &&other) noexcept;

  VkAccelerationStructureKHR GetAccelerationStructure() const;

  static std::vector<VkAccelerationStructureKHR> BuildAccelerationStructures(VkBuffer main_buffer, VkDeviceAddress scratch_buffer,
                                                                             std::span<const AccelerationInformation> acceleration_information,
                                                                             std::span<const BottomLevelGeometry> bottom_geometries);

  static VkAccelerationStructureKHR BuildAccelerationStructures(VkBuffer main_buffer, VkDeviceAddress scratch_buffer,
                                                                const AccelerationInformation &acceleration_information,
                                                                std::span<const BottomLevelAccelerationStructureInstances> bottom_instances);

protected:
private:
  VkAccelerationStructureKHR acceleration_structure_{VK_NULL_HANDLE};
  VkBuffer acceleration_buffer_{VK_NULL_HANDLE};
  VmaAllocation buffer_allocation_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_ACCELERATION_STRUCTURE_H