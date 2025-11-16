#ifndef INNSMOUTH_SWAPCHAIN_H
#define INNSMOUTH_SWAPCHAIN_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"
#include <span>

struct GLFWwindow;

namespace Innsmouth {

class Swapchain {
public:
  Swapchain(GLFWwindow *native_window);

  const VkExtent2D &GetExtent() const;
  Format GetFormat() const;

  std::span<const VkImageView> GetImageViews() const;
  std::size_t GetImageCount() const;
  const VkImageView GetCurrentImageView() const;

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
  VkSurfaceFormatKHR surface_format_;
  VkExtent2D surface_extent_;
  std::vector<VkImage> images_;
  std::vector<VkImageView> image_views_;
  uint32_t current_image_{0};
};

} // namespace Innsmouth

#endif // INNSMOUTH_SWAPCHAIN_H