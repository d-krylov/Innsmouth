#ifndef INNSMOUTH_SWAPCHAIN_H
#define INNSMOUTH_SWAPCHAIN_H

#include "innsmouth/graphics/include/graphics_types.h"
#include <vector>

namespace Innsmouth {

class Window;
class Semaphore;

class Swapchain {
public:
  Swapchain(Window &window);

  NO_COPY_SEMANTIC(Swapchain);

  ~Swapchain();

  operator const VkSwapchainKHR &() const { return swapchain_; }
  [[nodiscard]] const VkSwapchainKHR *get() const { return &swapchain_; }

  [[nodiscard]] const std::vector<VkImage> &GetImages() const { return images_; }
  [[nodiscard]] const std::vector<VkImageView> &GetImageViews() const { return image_views_; }

  [[nodiscard]] const VkSurfaceFormatKHR &GetSurfaceFormat() const { return surface_format_; }
  [[nodiscard]] const VkSurfaceCapabilitiesKHR &GetSurfaceCapabilities() const {
    return surface_capabilities_;
  }

  [[nodiscard]] std::size_t GetImageCount() const { return images_.size(); }
  [[nodiscard]] uint32_t GetCurrentImageIndex() const { return current_image_; }

  [[nodiscard]] VkResult Present(const Semaphore &semaphore);

  [[nodiscard]] VkResult AcquireNextImage(const Semaphore &semaphore);

  void Cleanup();
  void Recreate();

protected:
  void CreateSurface(Window &window);
  void CreateSwapchain();
  void CreateImages();
  void CreateImageViews();

private:
  VkSurfaceKHR surface_{VK_NULL_HANDLE};
  VkSurfaceFormatKHR surface_format_{};
  VkSurfaceCapabilitiesKHR surface_capabilities_;
  VkSwapchainKHR swapchain_{VK_NULL_HANDLE};
  VkSwapchainKHR swapchain_previous_{VK_NULL_HANDLE};
  std::vector<VkImage> images_;
  std::vector<VkImageView> image_views_;
  uint32_t current_image_{0};
};

} // namespace Innsmouth

#endif // INNSMOUTH_SWAPCHAIN_H