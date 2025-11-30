#include "swapchain.h"
#include "innsmouth/graphics/command/command_buffer.h"
#include "innsmouth/graphics/synchronization/fence.h"
#include "innsmouth/graphics/core/structure_tools.h"
#include "innsmouth/graphics/image/image.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <limits>
#include <span>

namespace Innsmouth {

SurfaceFormatKHR SelectSurfaceFormat(const VkSurfaceKHR surface, std::span<const SurfaceFormatKHR> required_formats) {
  auto supported = Enumerate<SurfaceFormatKHR>(vkGetPhysicalDeviceSurfaceFormatsKHR, GraphicsContext::Get()->GetPhysicalDevice(), surface);
  auto it = std::ranges::find_first_of(supported, required_formats);
  return (it != supported.end()) ? *it : supported[0];
}

PresentModeKHR SelectPresentMode(const VkSurfaceKHR surface, std::span<const PresentModeKHR> required_modes) {
  auto supported = Enumerate<PresentModeKHR>(vkGetPhysicalDeviceSurfacePresentModesKHR, GraphicsContext::Get()->GetPhysicalDevice(), surface);
  auto it = std::ranges::find_first_of(required_modes, supported);
  return (it != required_modes.end()) ? *it : PresentModeKHR::E_FIFO_KHR;
}

uint32_t ComputeImageCount(const VkSurfaceCapabilitiesKHR &capabilities) {
  constexpr auto infinity = std::numeric_limits<uint32_t>::max();
  return std::min(capabilities.minImageCount + 1, capabilities.maxImageCount > 0 ? capabilities.maxImageCount : infinity);
}

SurfaceCapabilitiesKHR GetSurfaceCapabilities(const VkSurfaceKHR surface) {
  SurfaceCapabilitiesKHR surface_capabilities;
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GraphicsContext::Get()->GetPhysicalDevice(), surface, surface_capabilities));
  return surface_capabilities;
}

void Swapchain::CreateSwapchain() {
  auto surface_capabilities = GetSurfaceCapabilities(surface_);
  auto image_count = ComputeImageCount(surface_capabilities);

  surface_extent_.width = surface_capabilities.currentExtent.width;
  surface_extent_.height = surface_capabilities.currentExtent.height;

  std::array required_present_modes{PresentModeKHR::E_MAILBOX_KHR};

  SwapchainCreateInfoKHR swapchain_ci{};

  swapchain_ci.surface = surface_;
  swapchain_ci.compositeAlpha = CompositeAlphaMaskBitsKHR::E_OPAQUE_BIT_KHR;
  swapchain_ci.clipped = true;
  swapchain_ci.preTransform = surface_capabilities.currentTransform;
  swapchain_ci.imageExtent = surface_capabilities.currentExtent;
  swapchain_ci.imageArrayLayers = 1;
  swapchain_ci.imageSharingMode = SharingMode::E_EXCLUSIVE;
  swapchain_ci.oldSwapchain = swapchain_previous_;
  swapchain_ci.imageUsage = ImageUsageMaskBits::E_COLOR_ATTACHMENT_BIT;
  swapchain_ci.imageFormat = surface_format_.format;
  swapchain_ci.imageColorSpace = surface_format_.colorSpace;
  swapchain_ci.minImageCount = ComputeImageCount(surface_capabilities);
  swapchain_ci.presentMode = SelectPresentMode(surface_, required_present_modes);

  images_.resize(image_count);

  VK_CHECK(vkCreateSwapchainKHR(GraphicsContext::Get()->GetDevice(), swapchain_ci, nullptr, &swapchain_current_));
  VK_CHECK(vkGetSwapchainImagesKHR(GraphicsContext::Get()->GetDevice(), swapchain_current_, &image_count, images_.data()));
}

void Swapchain::CreateImageViews() {
  image_views_.resize(images_.size());
  auto subresource_range = GetImageSubresourceRange();
  for (auto i = 0; i < images_.size(); i++) {
    image_views_[i] = Image::CreateImageView(images_[i], GetFormat(), ImageViewType::E_2D, subresource_range);
  }
}

void Swapchain::CreateSurface(GLFWwindow *native_window) {
  VK_CHECK(glfwCreateWindowSurface(GraphicsContext::Get()->GetInstance(), native_window, nullptr, &surface_));

  std::vector<SurfaceFormatKHR> required_formats{{Format::E_R8G8B8A8_SRGB, ColorSpaceKHR::E_SRGB_NONLINEAR_KHR},
                                                 {Format::E_B8G8R8A8_SRGB, ColorSpaceKHR::E_SRGB_NONLINEAR_KHR}};

  surface_format_ = SelectSurfaceFormat(surface_, required_formats);
}

const Extent2D &Swapchain::GetExtent() const {
  return surface_extent_;
}

Format Swapchain::GetFormat() const {
  return surface_format_.format;
}

std::span<const VkImageView> Swapchain::GetImageViews() const {
  return image_views_;
}

uint32_t Swapchain::GetImageCount() const {
  return image_views_.size();
}

uint32_t Swapchain::GetCurrentImageIndex() const {
  return current_image_index_;
}

VkImageView Swapchain::GetCurrentImageView() const {
  return image_views_[current_image_index_];
}

VkImage Swapchain::GetCurrentImage() const {
  return images_[current_image_index_];
}

void Swapchain::Cleanup() {
  for (const auto &image_view : image_views_) {
    vkDestroyImageView(GraphicsContext::Get()->GetDevice(), image_view, nullptr);
  }
  vkDestroySwapchainKHR(GraphicsContext::Get()->GetDevice(), swapchain_current_, nullptr);
}

void Swapchain::Recreate() {
  vkDeviceWaitIdle(GraphicsContext::Get()->GetDevice());
  Cleanup();
  CreateSwapchain();
  CreateImageViews();
}

Swapchain::Swapchain(GLFWwindow *native_window) {
  CreateSurface(native_window);
  CreateSwapchain();
  CreateImageViews();
}

VkResult Swapchain::Present(const VkSemaphore *wait_semaphore) {
  PresentInfoKHR present_info;

  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = wait_semaphore;
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &swapchain_current_;
  present_info.pImageIndices = &current_image_index_;

  return vkQueuePresentKHR(GraphicsContext::Get()->GetGraphicsQueue(), present_info);
}

VkResult Swapchain::AcquireNextImage(const VkSemaphore semaphore) {
  auto ret =
    vkAcquireNextImageKHR(GraphicsContext::Get()->GetDevice(), swapchain_current_, UINT64_MAX, semaphore, nullptr, &current_image_index_);
  return ret;
}

} // namespace Innsmouth