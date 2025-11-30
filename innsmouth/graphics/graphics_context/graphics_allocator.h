#ifndef INNSMOUTH_GRAPHICS_ALLOCATOR_H
#define INNSMOUTH_GRAPHICS_ALLOCATOR_H

#include "innsmouth/graphics/core/graphics_types.h"
#include <span>

namespace Innsmouth {

struct AllocationInformation {
  VmaAllocation allocation_{nullptr};
  std::byte *mapped_memory_{nullptr};
};

class GraphicsAllocator {
public:
  GraphicsAllocator();

  ~GraphicsAllocator();

  AllocationInformation AllocateImage(const VkImageCreateInfo &image_ci, VkImage &image);
  AllocationInformation AllocateBuffer(const BufferCreateInfo &buffer_ci, VkBuffer &out_buffer, AllocationCreateMask allocation_mask);

  static GraphicsAllocator *Get();

  void CopyMemoryToAllocation(std::span<const std::byte> source, VmaAllocation destination, std::size_t offset);

  void MapMemory(VmaAllocation allocation, std::byte **mapped_memory);

  void UnmapMemory(VmaAllocation allocation);

  void DestroyImage(VkImage image, VmaAllocation allocation);

  void DestroyBuffer(VkBuffer buffer, VmaAllocation allocation);

protected:
  void CreateAllocator();

private:
  VmaAllocator vma_allocator_;

  static GraphicsAllocator *graphics_allocator_instance_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_ALLOCATOR_H