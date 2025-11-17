#ifndef INNSMOUTH_GRAPHICS_ALLOCATOR_H
#define INNSMOUTH_GRAPHICS_ALLOCATOR_H

#include <vma/vk_mem_alloc.h>
#include "innsmouth/graphics/core/graphics_structures.h"
#include <span>

namespace Innsmouth {

class GraphicsAllocator {
public:
  GraphicsAllocator();

  ~GraphicsAllocator();

  VmaAllocation AllocateImage(const VkImageCreateInfo &image_ci, VkImage *image);

  VmaAllocation AllocateBuffer(const BufferCreateInfo &buffer_ci, VkBuffer &out_buffer);

  static GraphicsAllocator *Get();

  void CopyMemoryToAllocation(std::span<const std::byte> source, VmaAllocation destination, std::size_t offset);

  void MapMemory(VmaAllocation allocation, std::byte **mapped_memory);

  void UnmapMemory(VmaAllocation allocation);

protected:
  void CreateAllocator();

private:
  VmaAllocator vma_allocator_;

  static GraphicsAllocator *graphics_allocator_instance_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_ALLOCATOR_H