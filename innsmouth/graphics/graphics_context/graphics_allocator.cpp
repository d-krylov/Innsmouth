#include "graphics_allocator.h"
#include "graphics_context.h"
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace Innsmouth {

GraphicsAllocator *GraphicsAllocator::graphics_allocator_instance_ = nullptr;

GraphicsAllocator *GraphicsAllocator::Get() {
  return graphics_allocator_instance_;
}

GraphicsAllocator::GraphicsAllocator() {
  CreateAllocator();
  graphics_allocator_instance_ = this;
}

GraphicsAllocator::~GraphicsAllocator() {
}

void GraphicsAllocator::CreateAllocator() {
  VmaVulkanFunctions vulkan_functions{};
  {
    vulkan_functions.vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)vkGetInstanceProcAddr;
    vulkan_functions.vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)vkGetDeviceProcAddr;
  }

  VmaAllocatorCreateInfo allocator_ci{};
  {
    allocator_ci.instance = GraphicsContext::Get()->GetInstance();
    allocator_ci.physicalDevice = GraphicsContext::Get()->GetPhysicalDevice();
    allocator_ci.device = GraphicsContext::Get()->GetDevice();
    allocator_ci.vulkanApiVersion = VK_API_VERSION_1_3;
    allocator_ci.pVulkanFunctions = &vulkan_functions;
    allocator_ci.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
  }

  VK_CHECK(vmaCreateAllocator(&allocator_ci, &vma_allocator_));
}

VmaAllocation GraphicsAllocator::AllocateImage(const VkImageCreateInfo &image_ci, VkImage *image) {
  VmaAllocationCreateInfo vma_allocation_ci{};
  {
    vma_allocation_ci.flags = 0;
    vma_allocation_ci.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    vma_allocation_ci.requiredFlags = 0;
    vma_allocation_ci.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    vma_allocation_ci.memoryTypeBits = 0;
    vma_allocation_ci.pool = nullptr;
    vma_allocation_ci.pUserData = nullptr;
  }

  VmaAllocation allocation{VK_NULL_HANDLE};
  VmaAllocationInfo allocation_info{};

  VK_CHECK(vmaCreateImage(vma_allocator_, &image_ci, &vma_allocation_ci, image, &allocation, &allocation_info));

  return allocation;
}

VmaAllocation GraphicsAllocator::AllocateBuffer(const BufferCreateInfo &buffer_ci, VkBuffer &out_buffer) {
  VmaAllocationCreateInfo vma_allocation_ci{};
  {
    vma_allocation_ci.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    vma_allocation_ci.usage = VMA_MEMORY_USAGE_AUTO;
    vma_allocation_ci.requiredFlags = 0;
    vma_allocation_ci.preferredFlags = 0;
    vma_allocation_ci.memoryTypeBits = 0;
    vma_allocation_ci.pool = nullptr;
    vma_allocation_ci.pUserData = nullptr;
  }

  VmaAllocation allocation{VK_NULL_HANDLE};
  VmaAllocationInfo allocation_info{};

  VK_CHECK(vmaCreateBuffer(vma_allocator_, buffer_ci, &vma_allocation_ci, &out_buffer, &allocation, &allocation_info));

  return allocation;
}

void GraphicsAllocator::MapMemory(VmaAllocation allocation, std::byte **mapped_memory) {
  VK_CHECK(vmaMapMemory(vma_allocator_, allocation, reinterpret_cast<void **>(mapped_memory)));
}

void GraphicsAllocator::UnmapMemory(VmaAllocation allocation) {
  vmaUnmapMemory(vma_allocator_, allocation);
}

void GraphicsAllocator::CopyMemoryToAllocation(std::span<const std::byte> source, VmaAllocation destination, std::size_t offset) {
  vmaCopyMemoryToAllocation(vma_allocator_, source.data(), destination, offset, source.size());
}

} // namespace Innsmouth