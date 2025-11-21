#ifndef INNSMOUTH_ACCELERATION_STRUCTURE_H
#define INNSMOUTH_ACCELERATION_STRUCTURE_H

#include "innsmouth/mathematics/include/mathematics_types.h"
#include "innsmouth/graphics/buffer/buffer.h"
#include <vector>

namespace Innsmouth {

struct BLASSpecification {
  uint32_t vertices_offset_;
  uint32_t vertices_number_;
  uint32_t indices_number_;
  uint32_t indices_offset_;
};

struct TLASInstance {
  Matrix4f transform_ = Matrix4f(1.0f);
};

class AccelerationStructure {
public:
  AccelerationStructure() = default;

  AccelerationStructure(VkDeviceAddress vbo_address, VkDeviceAddress ibo_address, std::size_t vertex_stride,
                        std::span<const BLASSpecification> specifications);

  AccelerationStructure(std::span<const VkAccelerationStructureKHR> blases, std::span<const TLASInstance> instances);

  std::span<const VkAccelerationStructureKHR> GetAccelerationStructures() const;

protected:
  VkAccelerationStructureKHR CreateTopLevelAccelerationStructure(std::span<const VkAccelerationStructureKHR> blases,
                                                                 std::span<const TLASInstance> instances);

  std::vector<VkAccelerationStructureKHR> CreateBottomLevelAccelerationStructure(VkDeviceAddress vbo_address, VkDeviceAddress ibo_address,
                                                                                 std::size_t vertex_stride,
                                                                                 std::span<const BLASSpecification> specifications);

private:
  std::vector<VkAccelerationStructureKHR> acceleration_structures_{VK_NULL_HANDLE};
  Buffer acceleration_structures_buffer_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_ACCELERATION_STRUCTURE_H