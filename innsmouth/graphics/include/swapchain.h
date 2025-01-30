#ifndef INNSMOUTH_SWAPCHAIN_H
#define INNSMOUTH_SWAPCHAIN_H

#include "graphics_types.h"
#include <vector>

namespace Innsmouth {

class Swapchain {
public:
protected:
  void CreateSwapchain();

private:
  VkSurfaceKHR surface_{VK_NULL_HANDLE};
  VkSwapchainKHR swapchain_{VK_NULL_HANDLE};
  VkSwapchainKHR swapchain_previous_{VK_NULL_HANDLE};
  std::vector<VkImage> images_;
  std::vector<VkImageView> image_views_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_SWAPCHAIN_H