#include "acceleration_structure.h"
#include "innsmouth/core/include/core.h"
#include "innsmouth/graphics/command/command_buffer.h"
#include <print>

namespace Innsmouth {

TransformMatrixKHR ConvertTransform(const Matrix4f &matrix) {
  TransformMatrixKHR out{};

  out.matrix[0][0] = matrix[0][0];
  out.matrix[0][1] = matrix[0][1];
  out.matrix[0][2] = matrix[0][2];
  out.matrix[0][3] = matrix[0][3];

  out.matrix[1][0] = matrix[1][0];
  out.matrix[1][1] = matrix[1][1];
  out.matrix[1][2] = matrix[1][2];
  out.matrix[1][3] = matrix[1][3];

  out.matrix[2][0] = matrix[2][0];
  out.matrix[2][1] = matrix[2][1];
  out.matrix[2][2] = matrix[2][2];
  out.matrix[2][3] = matrix[2][3];

  return out;
}

std::vector<VkAccelerationStructureKHR>
AccelerationStructure::CreateBottomLevelAccelerationStructure(VkDeviceAddress vbo_address, VkDeviceAddress ibo_address,
                                                              std::size_t vertex_stride, std::span<const BLASSpecification> specifications) {

  auto primitives_count = specifications.size();

  std::vector<AccelerationStructureGeometryKHR> geometries(primitives_count);
  std::vector<AccelerationStructureBuildGeometryInfoKHR> build_geometry_infos(primitives_count);

  std::vector<std::size_t> acceleration_offsets(primitives_count);
  std::vector<std::size_t> acceleration_sizes(primitives_count);
  std::vector<std::size_t> scratch_offsets(primitives_count);
  std::vector<std::size_t> scratch_sizes(primitives_count);
  std::vector<uint32_t> triangles_count(primitives_count);

  std::size_t total_acceleration_size = 0;
  std::size_t total_scratch_size = 0;

  std::size_t alignment = 256;

  for (auto i = 0; i < primitives_count; i++) {
    const auto &specification = specifications[i];
    auto &build_geometry_info = build_geometry_infos[i];
    auto &geometry = geometries[i];

    geometry.geometryType = GeometryTypeKHR::E_TRIANGLES_KHR;
    geometry.flags = GeometryMaskBitsKHR::E_OPAQUE_BIT_KHR;
    geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
    geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
    geometry.geometry.triangles.vertexData.deviceAddress = vbo_address + specification.vertices_offset_ * vertex_stride;
    geometry.geometry.triangles.maxVertex = specification.vertices_number_ - 1;
    geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
    geometry.geometry.triangles.indexData.deviceAddress = ibo_address + specification.indices_offset_ * sizeof(uint32_t);
    geometry.geometry.triangles.vertexStride = vertex_stride;

    build_geometry_info.flags = BuildAccelerationStructureMaskBitsKHR::E_PREFER_FAST_TRACE_BIT_KHR;
    build_geometry_info.mode = BuildAccelerationStructureModeKHR::E_BUILD_KHR;
    build_geometry_info.type = AccelerationStructureTypeKHR::E_BOTTOM_LEVEL_KHR;
    build_geometry_info.geometryCount = 1;
    build_geometry_info.pGeometries = &geometry;

    triangles_count[i] = specification.indices_number_ / 3;

    AccelerationStructureBuildSizesInfoKHR build_sizes_info;
    vkGetAccelerationStructureBuildSizesKHR(GraphicsContext::Get()->GetDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
                                            build_geometry_info, &triangles_count[i], build_sizes_info);

    acceleration_offsets[i] = total_acceleration_size;
    acceleration_sizes[i] = build_sizes_info.accelerationStructureSize;
    scratch_offsets[i] = total_scratch_size;

    total_acceleration_size = AlignUp(total_acceleration_size + build_sizes_info.accelerationStructureSize, alignment);
    total_scratch_size = AlignUp(total_scratch_size + build_sizes_info.buildScratchSize, alignment);
  }

  BufferUsageMask blas_usage = BufferUsageMaskBits::E_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | BufferUsageMaskBits::E_SHADER_DEVICE_ADDRESS_BIT;
  BufferUsageMask scratch_usage = BufferUsageMaskBits::E_SHADER_DEVICE_ADDRESS_BIT | BufferUsageMaskBits::E_STORAGE_BUFFER_BIT;
  Buffer scratch_buffer(total_scratch_size, scratch_usage, AllocationCreateMaskBits::E_DEDICATED_MEMORY_BIT);
  acceleration_structures_buffer_ = Buffer(total_acceleration_size, blas_usage, AllocationCreateMaskBits::E_DEDICATED_MEMORY_BIT);

  std::vector<AccelerationStructureBuildRangeInfoKHR> build_ranges(primitives_count);
  std::vector<const AccelerationStructureBuildRangeInfoKHR *> build_range_pointers(primitives_count);

  std::vector<VkAccelerationStructureKHR> acceleration_structures(primitives_count, VK_NULL_HANDLE);

  auto scratch_address = scratch_buffer.GetBufferAddress();

  for (auto i = 0; i < primitives_count; i++) {
    AccelerationStructureCreateInfoKHR acceleration_structure_ci;

    acceleration_structure_ci.buffer = acceleration_structures_buffer_.GetHandle();
    acceleration_structure_ci.offset = acceleration_offsets[i];
    acceleration_structure_ci.size = acceleration_sizes[i];
    acceleration_structure_ci.type = AccelerationStructureTypeKHR::E_BOTTOM_LEVEL_KHR;

    VK_CHECK(
      vkCreateAccelerationStructureKHR(GraphicsContext::Get()->GetDevice(), acceleration_structure_ci, nullptr, &acceleration_structures[i]));

    build_geometry_infos[i].dstAccelerationStructure = acceleration_structures[i];
    build_geometry_infos[i].scratchData.deviceAddress = scratch_address + scratch_offsets[i];

    build_ranges[i].primitiveCount = triangles_count[i];
    build_range_pointers[i] = &build_ranges[i];
  }

  CommandBuffer command_buffer(GraphicsContext::Get()->GetGraphicsQueueIndex());
  command_buffer.Begin();
  command_buffer.CommandBuildAccelerationStructure(build_geometry_infos, build_range_pointers);
  command_buffer.End();
  command_buffer.Submit();

  vkDeviceWaitIdle(GraphicsContext::Get()->GetDevice());

  return acceleration_structures;
}

VkAccelerationStructureKHR AccelerationStructure::CreateTopLevelAccelerationStructure(std::span<const VkAccelerationStructureKHR> blases,
                                                                                      std::span<const TLASInstance> instances) {

  std::vector<VkDeviceAddress> blas_addresses(blases.size());

  for (auto i = 0; i < blases.size(); ++i) {
    AccelerationStructureDeviceAddressInfoKHR device_address_info;
    device_address_info.accelerationStructure = blases[i];
    blas_addresses[i] = vkGetAccelerationStructureDeviceAddressKHR(GraphicsContext::Get()->GetDevice(), device_address_info);
  }

  auto instance_buffer_usage = BufferUsageMaskBits::E_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | //
                               BufferUsageMaskBits::E_SHADER_DEVICE_ADDRESS_BIT;

  auto instance_buffer_size = sizeof(AccelerationStructureInstanceKHR) * blases.size();

  Buffer instance_buffer(instance_buffer_size, instance_buffer_usage, AllocationCreateMaskBits::E_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

  std::vector<AccelerationStructureInstanceKHR> instances_vector;

  TransformMatrixKHR identity_transform = {{
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
  }};

  for (auto i = 0; i < blases.size(); ++i) {
    auto &instance = instances_vector.emplace_back();
    instance.transform = identity_transform;
    instance.instanceCustomIndex = i;
    instance.mask = 0xff;
    instance.accelerationStructureReference = blas_addresses[i];
  }

  instance_buffer.SetData<AccelerationStructureInstanceKHR>(instances_vector);

  AccelerationStructureGeometryKHR geometry;
  geometry.geometryType = GeometryTypeKHR::E_INSTANCES_KHR;
  geometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
  geometry.geometry.instances.data.deviceAddress = instance_buffer.GetBufferAddress();

  AccelerationStructureBuildGeometryInfoKHR build_geometry_info[1];
  build_geometry_info[0].type = AccelerationStructureTypeKHR::E_TOP_LEVEL_KHR;
  build_geometry_info[0].flags = BuildAccelerationStructureMaskBitsKHR::E_PREFER_FAST_TRACE_BIT_KHR;
  build_geometry_info[0].mode = BuildAccelerationStructureModeKHR::E_BUILD_KHR;
  build_geometry_info[0].geometryCount = 1;
  build_geometry_info[0].pGeometries = &geometry;

  uint32_t primitive_count = blases.size();

  AccelerationStructureBuildSizesInfoKHR build_sizes_info;
  vkGetAccelerationStructureBuildSizesKHR(GraphicsContext::Get()->GetDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
                                          build_geometry_info[0], &primitive_count, build_sizes_info);

  BufferUsageMask tlas_usage = BufferUsageMaskBits::E_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
  BufferUsageMask scratch_usage = BufferUsageMaskBits::E_STORAGE_BUFFER_BIT | BufferUsageMaskBits::E_SHADER_DEVICE_ADDRESS_BIT;
  Buffer scratch_buffer(build_sizes_info.buildScratchSize, scratch_usage, AllocationCreateMaskBits::E_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
  acceleration_structures_buffer_ = Buffer(build_sizes_info.accelerationStructureSize, tlas_usage, {});

  AccelerationStructureCreateInfoKHR acceleration_structure_ci;
  acceleration_structure_ci.buffer = acceleration_structures_buffer_.GetHandle();
  acceleration_structure_ci.size = build_sizes_info.accelerationStructureSize;
  acceleration_structure_ci.type = AccelerationStructureTypeKHR::E_TOP_LEVEL_KHR;

  VkAccelerationStructureKHR acceleration_structure = VK_NULL_HANDLE;
  VK_CHECK(vkCreateAccelerationStructureKHR(GraphicsContext::Get()->GetDevice(), acceleration_structure_ci, nullptr, &acceleration_structure));

  build_geometry_info[0].dstAccelerationStructure = acceleration_structure;
  build_geometry_info[0].scratchData.deviceAddress = scratch_buffer.GetBufferAddress();

  AccelerationStructureBuildRangeInfoKHR build_range;
  build_range.primitiveCount = primitive_count;
  const AccelerationStructureBuildRangeInfoKHR *build_range_pointer[1] = {&build_range};

  CommandBuffer command_buffer(GraphicsContext::Get()->GetGraphicsQueueIndex());
  command_buffer.Begin();
  command_buffer.CommandBuildAccelerationStructure(build_geometry_info, build_range_pointer);
  command_buffer.End();
  command_buffer.Submit();

  vkDeviceWaitIdle(GraphicsContext::Get()->GetDevice());

  return acceleration_structure;
}

AccelerationStructure::AccelerationStructure(VkDeviceAddress vbo_address, VkDeviceAddress ibo_address, std::size_t vertex_stride,
                                             std::span<const BLASSpecification> specifications) {
  acceleration_structures_ = CreateBottomLevelAccelerationStructure(vbo_address, ibo_address, vertex_stride, specifications);
}

AccelerationStructure::AccelerationStructure(std::span<const VkAccelerationStructureKHR> blases, std::span<const TLASInstance> instances) {
  acceleration_structures_.resize(1);
  acceleration_structures_[0] = CreateTopLevelAccelerationStructure(blases, instances);
}

std::span<const VkAccelerationStructureKHR> AccelerationStructure::GetAccelerationStructures() const {
  return acceleration_structures_;
}

} // namespace Innsmouth