#ifndef INNSMOUTH_SWAPCHAIN_H
#define INNSMOUTH_SWAPCHAIN_H

#include "graphics/include/graphics_types.h"
#include <vector>

namespace Innsmouth {

class Window;
class Semaphore;

class Swapchain {
public:
  Swapchain(Window &window);

  NO_COPY_SEMANTIC(Swapchain);

  ~Swapchain();

  auto begin() { return image_views_.begin(); }
  auto begin() const { return image_views_.begin(); }

  auto end() { return image_views_.end(); }
  auto end() const { return image_views_.end(); }

  operator const VkSwapchainKHR &() const { return swapchain_; }
  [[nodiscard]] const VkSwapchainKHR *get() const { return &swapchain_; }

  [[nodiscard]] Format GetSurfaceFormat() const { return surface_format_; }
  [[nodiscard]] const VkExtent2D &GetSurfaceExtent() const { return surface_extent_; }
  [[nodiscard]] uint32_t GetSurfaceWidth() const { return surface_extent_.width; }
  [[nodiscard]] uint32_t GetSurfaceHeight() const { return surface_extent_.height; }

  [[nodiscard]] std::size_t GetImageCount() const { return images_.size(); }
  [[nodiscard]] uint32_t GetCurrentImageIndex() const { return current_image_; }

  [[nodiscard]] VkResult Present(const Semaphore &semaphore);

  [[nodiscard]] VkResult AcquireNextImage(const Semaphore &semaphore);

  [[nodiscard]] const VkImageView GetCurrentImageView() const { return image_views_[current_image_]; }

  void Cleanup();
  void Recreate();

protected:
  void CreateSurface(Window &window);
  void CreateSwapchain();
  void CreateImageViews();

private:
  Format surface_format_;
  VkExtent2D surface_extent_;
  VkSurfaceKHR surface_{VK_NULL_HANDLE};
  VkSwapchainKHR swapchain_{VK_NULL_HANDLE};
  VkSwapchainKHR swapchain_previous_{VK_NULL_HANDLE};
  std::vector<VkImage> images_;
  std::vector<VkImageView> image_views_;
  uint32_t current_image_{0};
};

} // namespace Innsmouth

#endif // INNSMOUTH_SWAPCHAIN_H