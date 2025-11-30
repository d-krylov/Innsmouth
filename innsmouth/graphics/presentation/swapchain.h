#ifndef INNSMOUTH_SWAPCHAIN_H
#define INNSMOUTH_SWAPCHAIN_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"
#include <span>

struct GLFWwindow;

namespace Innsmouth {

class Swapchain {
public:
  Swapchain(GLFWwindow *native_window);

  Swapchain(const Swapchain &) = delete;
  Swapchain &operator=(const Swapchain &) = delete;

  const Extent2D &GetExtent() const;
  Format GetFormat() const;

  std::span<const VkImageView> GetImageViews() const;
  VkImageView GetCurrentImageView() const;
  VkImage GetCurrentImage() const;

  uint32_t GetCurrentImageIndex() const;
  uint32_t GetImageCount() const;

  VkResult Present(const VkSemaphore *wait_semaphore);
  VkResult AcquireNextImage(const VkSemaphore semaphore);

  void Recreate();

protected:
  void CreateSurface(GLFWwindow *native_window);
  void CreateSwapchain();
  void CreateImageViews();
  void Cleanup();

private:
  VkSurfaceKHR surface_{VK_NULL_HANDLE};
  VkSwapchainKHR swapchain_current_{VK_NULL_HANDLE};
  VkSwapchainKHR swapchain_previous_{VK_NULL_HANDLE};
  SurfaceFormatKHR surface_format_;
  Extent2D surface_extent_;
  std::vector<VkImage> images_;
  std::vector<VkImageView> image_views_;
  uint32_t current_image_index_{0};
};

} // namespace Innsmouth

#endif // INNSMOUTH_SWAPCHAIN_H