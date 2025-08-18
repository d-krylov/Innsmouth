#include "graphics_allocator.h"
#include "graphics_context.h"
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace Innsmouth {

GraphicsAllocator *GraphicsAllocator::graphics_allocator_instance_ = nullptr;

GraphicsAllocator &GraphicsAllocator::Get() {
  return *graphics_allocator_instance_;
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
    allocator_ci.instance = GraphicsContext::Get().GetInstance();
    allocator_ci.physicalDevice = GraphicsContext::Get().GetPhysicalDevice();
    allocator_ci.device = GraphicsContext::Get().GetDevice();
    allocator_ci.vulkanApiVersion = VK_API_VERSION_1_3;
    allocator_ci.pVulkanFunctions = &vulkan_functions;
    allocator_ci.flags = 0;
  }

  VK_CHECK(vmaCreateAllocator(&allocator_ci, &vma_allocator_));
}

VmaAllocation GraphicsAllocator::AllocateImage(const VkImageCreateInfo &image_ci, VkImage *image) {
  VmaAllocationCreateInfo allocation_ci{};
  {
    allocation_ci.flags = 0;
    allocation_ci.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    allocation_ci.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    allocation_ci.preferredFlags = 0;
    allocation_ci.memoryTypeBits = 0;
    allocation_ci.pool = nullptr;
    allocation_ci.pUserData = nullptr;
  }

  VmaAllocation allocation{VK_NULL_HANDLE};
  VmaAllocationInfo allocation_info{};

  VK_CHECK(vmaCreateImage(vma_allocator_, &image_ci, &allocation_ci, image, &allocation, &allocation_info));

  return allocation;
}

VmaAllocation GraphicsAllocator::AllocateBuffer(const VkBufferCreateInfo &buffer_ci, VkBuffer *buffer) {
  VmaAllocationCreateInfo vma_allocation_ci{};
  {
    vma_allocation_ci.usage = VMA_MEMORY_USAGE_AUTO;
    vma_allocation_ci.flags = 0;
  }

  VmaAllocation allocation{VK_NULL_HANDLE};
  VmaAllocationInfo allocation_info{};

  VK_CHECK(vmaCreateBuffer(vma_allocator_, &buffer_ci, &vma_allocation_ci, buffer, &allocation, &allocation_info));

  return allocation;
}

} // namespace Innsmouth