#include "graphics/include/swapchain.h"
#include "graphics/image/image.h"
#include "graphics/include/graphics.h"
#include "graphics/include/graphics_tools.h"
#include "graphics/include/synchronization.h"
#include "gui/include/window.h"
#include <algorithm>

namespace Innsmouth {

[[nodiscard]] uint32_t ComputeImageCount(const VkSurfaceCapabilitiesKHR &capabilities) {
  return std::min(capabilities.maxImageCount > 0 ? capabilities.maxImageCount : UINT32_MAX,
                  capabilities.minImageCount + 1);
}

VkSurfaceFormatKHR SelectSurfaceFormat(const VkPhysicalDevice device, const VkSurfaceKHR surface,
                                       const std::ranges::input_range auto &required, ColorSpace space) {
  auto supported = Enumerate<VkSurfaceFormatKHR>(vkGetPhysicalDeviceSurfaceFormatsKHR, device, surface);
  auto it = std::ranges::find_if(supported, [&](const VkSurfaceFormatKHR &f) {
    return std::ranges::find(required, Format(f.format)) != required.end() &&
           f.colorSpace == VkColorSpaceKHR(space);
  });
  return (it != supported.end()) ? *it : supported.front();
}

void Swapchain::CreateSurface(Window &window) {
  window.CreateSurface(Instance(), &surface_);
  std::vector<Format> required_formats{Format::B8G8R8A8_SRGB, Format::R8G8B8A8_SRGB};
  auto color_space = ColorSpace::SRGB_NONLINEAR_KHR;
  auto surface_format_khr = SelectSurfaceFormat(PhysicalDevice(), surface_, required_formats, color_space);
  surface_format_ = Format(surface_format_khr.format);
  VkBool32 is_present{VK_FALSE};
  vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice(), GraphicsIndex(), surface_, &is_present);
  CORE_VERIFY(is_present == VK_TRUE);
}

Swapchain::Swapchain(Window &window) {
  CreateSurface(window);
  CreateSwapchain();
  CreateImageViews();
}

Swapchain::~Swapchain() {
  vkDestroySwapchainKHR(Device(), swapchain_, nullptr);
  std::ranges::for_each(image_views_, [](auto &iv) { vkDestroyImageView(Device(), iv, nullptr); });
  vkDestroySurfaceKHR(Instance(), surface_, nullptr);
}

void Swapchain::CreateSwapchain() {
  auto supported =
    Enumerate<VkPresentModeKHR>(vkGetPhysicalDeviceSurfacePresentModesKHR, PhysicalDevice(), surface_);

  std::vector<VkPresentModeKHR> required{VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR};

  VkSurfaceCapabilitiesKHR surface_capabilities;
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice(), surface_, &surface_capabilities));
  surface_extent_ = surface_capabilities.currentExtent;

  auto image_count = ComputeImageCount(surface_capabilities);

  auto it = std::find_first_of(required.begin(), required.end(), supported.begin(), supported.end());

  VkSwapchainCreateInfoKHR swapchain_ci{};
  {
    swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_ci.surface = surface_;
    swapchain_ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_ci.clipped = true;
    swapchain_ci.preTransform = surface_capabilities.currentTransform;
    swapchain_ci.imageExtent = surface_capabilities.currentExtent;
    swapchain_ci.imageArrayLayers = 1;
    swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_ci.oldSwapchain = swapchain_previous_;
    swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_ci.imageFormat = VkFormat(surface_format_);
    swapchain_ci.imageColorSpace = VkColorSpaceKHR(ColorSpace::SRGB_NONLINEAR_KHR);
    swapchain_ci.minImageCount = image_count;
    swapchain_ci.presentMode = (it != required.end()) ? *it : VK_PRESENT_MODE_FIFO_KHR;
  }

  if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
    swapchain_ci.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }

  if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
    swapchain_ci.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }

  VK_CHECK(vkCreateSwapchainKHR(Device(), &swapchain_ci, nullptr, &swapchain_));

  VK_CHECK(vkGetSwapchainImagesKHR(Device(), swapchain_, &image_count, nullptr));

  images_.resize(image_count);

  VK_CHECK(vkGetSwapchainImagesKHR(Device(), swapchain_, &image_count, images_.data()));
}

void Swapchain::CreateImageViews() {
  image_views_.resize(images_.size());

  auto range = CreateImageSubresourceRange();

  for (uint32_t i = 0; i < image_views_.size(); i++) {
    Image::CreateImageView(images_[i], image_views_[i], surface_format_, ImageViewType::_2D, range);

    Image::TransitionImageLayout(images_[i], ImageLayout::UNDEFINED, ImageLayout::PRESENT_SRC_KHR,
                                 PipelineStage::TOP_OF_PIPE, PipelineStage::COLOR_ATTACHMENT_OUTPUT, range);
  }
}

VkResult Swapchain::AcquireNextImage(const Semaphore &semaphore) {
  auto ret = vkAcquireNextImageKHR(Device(), swapchain_, UINT64_MAX, semaphore, nullptr, &current_image_);
  return ret;
}

void Swapchain::Cleanup() {
  std::ranges::for_each(image_views_, [](auto &iv) { vkDestroyImageView(Device(), iv, nullptr); });
  vkDestroySwapchainKHR(Device(), swapchain_, nullptr);
}

void Swapchain::Recreate() {
  WaitIdle();
  Cleanup();
  CreateSwapchain();
  CreateImageViews();
}

VkResult Swapchain::Present(const Semaphore &wait_semaphore) {
  VkPresentInfoKHR present_info{};
  {
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = wait_semaphore.get();
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain_;
    present_info.pImageIndices = &current_image_;
  }
  return vkQueuePresentKHR(GraphicsQueue(), &present_info);
}

} // namespace Innsmouth