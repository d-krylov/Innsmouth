#include "acceleration_structure.h"

namespace Innsmouth {

BufferUsageMask scratch_usage = BufferUsageMaskBits::E_SHADER_DEVICE_ADDRESS_BIT | BufferUsageMaskBits::E_STORAGE_BUFFER_BIT;
BufferUsageMask blas_usage = BufferUsageMaskBits::E_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | BufferUsageMaskBits::E_SHADER_DEVICE_ADDRESS_BIT;

AccelerationStructure::AccelerationStructure(const BottomLevelGeometry &bottom_geometry) {
  auto acceleration_structure_sizes = GetAccelerationStructureSize(bottom_geometry);
  auto main_size = acceleration_structure_sizes.accelerationStructureSize;
  auto scratch_size = acceleration_structure_sizes.buildScratchSize;
  auto buffer_information = Buffer::CreateBuffer(main_size, blas_usage, AllocationCreateMaskBits::E_DEDICATED_MEMORY_BIT);
  acceleration_buffer_ = buffer_information.buffer_;
  buffer_allocation_ = buffer_information.buffer_allocation_;
  Buffer scrath_buffer(scratch_size, scratch_usage, AllocationCreateMaskBits::E_DEDICATED_MEMORY_BIT);
  std::array<AccelerationInformation, 1> acceleration_information;
  acceleration_information[0].acceleration_offset_ = 0;
  acceleration_information[0].scratch_offset_ = 0;
  acceleration_information[0].acceleration_size_ = main_size;
  auto acceleration_structures = BuildAccelerationStructures(acceleration_buffer_, scrath_buffer.GetBufferAddress(), acceleration_information,
                                                             std::span(&bottom_geometry, 1));

  acceleration_structure_ = acceleration_structures[0];
}

AccelerationStructure::AccelerationStructure(std::span<const BottomLevelAccelerationStructureInstances> bottom_instances) {
  uint32_t primitive_count = GetTotalInstancesCount(bottom_instances);
  auto acceleration_structure_sizes = GetAccelerationStructureSize(primitive_count);
  auto main_size = acceleration_structure_sizes.accelerationStructureSize;
  auto scratch_size = acceleration_structure_sizes.buildScratchSize;
  auto buffer_information = Buffer::CreateBuffer(main_size, blas_usage, AllocationCreateMaskBits::E_DEDICATED_MEMORY_BIT);
  acceleration_buffer_ = buffer_information.buffer_;
  buffer_allocation_ = buffer_information.buffer_allocation_;
  Buffer scrath_buffer(scratch_size, scratch_usage, AllocationCreateMaskBits::E_DEDICATED_MEMORY_BIT);
  AccelerationInformation acceleration_information;
  acceleration_information.acceleration_offset_ = 0;
  acceleration_information.scratch_offset_ = 0;
  acceleration_information.acceleration_size_ = main_size;
  acceleration_structure_ =
    BuildAccelerationStructures(acceleration_buffer_, scrath_buffer.GetBufferAddress(), acceleration_information, bottom_instances);
}

AccelerationStructure::AccelerationStructure(AccelerationStructure &&other) noexcept {
  acceleration_structure_ = std::exchange(other.acceleration_structure_, VK_NULL_HANDLE);
  acceleration_buffer_ = std::exchange(other.acceleration_buffer_, VK_NULL_HANDLE);
  buffer_allocation_ = std::exchange(other.buffer_allocation_, VK_NULL_HANDLE);
}

AccelerationStructure::~AccelerationStructure() {
  vkDestroyAccelerationStructureKHR(GraphicsContext::Get()->GetDevice(), acceleration_structure_, nullptr);
  GraphicsAllocator::Get()->DestroyBuffer(acceleration_buffer_, buffer_allocation_);
}

AccelerationStructure &AccelerationStructure::operator=(AccelerationStructure &&other) noexcept {
  std::swap(acceleration_structure_, other.acceleration_structure_);
  std::swap(acceleration_buffer_, other.acceleration_buffer_);
  std::swap(buffer_allocation_, other.buffer_allocation_);
  return *this;
}

VkAccelerationStructureKHR AccelerationStructure::GetAccelerationStructure() const {
  return acceleration_structure_;
}

} // namespace Innsmouth