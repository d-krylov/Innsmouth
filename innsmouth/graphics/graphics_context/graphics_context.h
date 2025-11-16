#ifndef INNSMOUTH_GRAPHICS_CONTEXT_H
#define INNSMOUTH_GRAPHICS_CONTEXT_H

#include "graphics_tools.h"
#include "innsmouth/graphics/core/graphics_structures.h"

namespace Innsmouth {

class GraphicsContext {
public:
  GraphicsContext();

  ~GraphicsContext();

  const VkInstance GetInstance() const;
  const VkPhysicalDevice GetPhysicalDevice() const;
  const VkDevice GetDevice() const;

  const VkQueue GetGeneralQueue() const;
  const VkCommandPool GetGeneralCommandPool() const;

  static GraphicsContext *Get();

protected:
  void CreateInstance();
  void PickPhysicalDevice();
  void CreateDevice();
  void CreateCommandPool();

  std::vector<const char *> GetInstanceLayers() const;

private:
  VkInstance instance_{VK_NULL_HANDLE};
  VkDebugUtilsMessengerEXT debug_messenger_{VK_NULL_HANDLE};
  VkPhysicalDevice physical_device_{VK_NULL_HANDLE};
  VkDevice device_{VK_NULL_HANDLE};
  int32_t general_queue_index_{-1};
  int32_t compute_queue_index_{-1};
  int32_t transfer_queue_index_{-1};
  VkQueue general_queue_{VK_NULL_HANDLE};
  VkQueue compute_queue_{VK_NULL_HANDLE};
  VkQueue transfer_queue_{VK_NULL_HANDLE};
  VkCommandPool general_command_pool_{VK_NULL_HANDLE};
  static GraphicsContext *graphics_context_instance_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_GRAPHICS_CONTEXT_H