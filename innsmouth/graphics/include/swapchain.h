#ifndef INNSMOUTH_SWAPCHAIN_H
#define INNSMOUTH_SWAPCHAIN_H

#include "graphics_types.h"
#include <vector>

namespace Innsmouth {

class Semaphore;
class Window;

class Swapchain {
public:
  Swapchain(const Window &window);

  VkSurfaceCapabilitiesKHR GetSurfaceCapabilities() const;
  VkExtent2D GetSurfaceExtent() const;

  VkResult AcquireNextImage(const VkSemaphore semaphore);
  VkResult Present(const VkSemaphore *wait_semaphore);

  const VkImageView GetCurrentImageView() const { return image_views_[current_image_]; }

  uint32_t GetCurrentImageIndex() const { return current_image_; }

  uint32_t GetImageCount() const { return images_.size(); }
  Format GetSurfaceFormat() const { return surface_format_; };

  void Recreate();

  auto begin() { return image_views_.begin(); }
  auto begin() const { return image_views_.begin(); }

  auto end() { return image_views_.end(); }
  auto end() const { return image_views_.end(); }

protected:
  void CreateSwapchain();
  void CreateImageViews();
  void Cleanup();

private:
  VkSurfaceKHR surface_{VK_NULL_HANDLE};
  VkSwapchainKHR swapchain_{VK_NULL_HANDLE};
  VkSwapchainKHR swapchain_previous_{VK_NULL_HANDLE};
  Format surface_format_;
  std::vector<VkImage> images_;
  std::vector<VkImageView> image_views_;
  uint32_t current_image_{0};
};

} // namespace Innsmouth

#endif // INNSMOUTH_SWAPCHAIN_H