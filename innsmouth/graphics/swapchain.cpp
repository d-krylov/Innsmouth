#include "graphics/include/swapchain.h"
#include "graphics/include/graphics.h"
#include "graphics/include/graphics_tools.h"
#include "graphics/include/structure_tools.h"
#include "graphics/buffer/command_buffer.h"
#include "graphics/image/image.h"
#include "gui/include/window.h"
#include <algorithm>
#include <ranges>
#include <limits>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Innsmouth {

VkSurfaceFormatKHR SelectSurfaceFormat(const VkSurfaceKHR surface, std::span<const Format> required_formats) {
  auto supported_formats = Enumerate(vkGetPhysicalDeviceSurfaceFormatsKHR, PhysicalDevice(), surface);
  auto cmp = [](auto &&s, auto &&r) { return (s.format == r && s.colorSpace == VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR); };
  auto it = std::ranges::find_first_of(supported_formats, required_formats, cmp);
  return it != supported_formats.end() ? *it : supported_formats[0];
}

VkPresentModeKHR SelectPresentMode(const VkSurfaceKHR surface, std::span<const VkPresentModeKHR> required_modes) {
  auto supported_modes = Enumerate(vkGetPhysicalDeviceSurfacePresentModesKHR, PhysicalDevice(), surface);
  auto it = std::ranges::find_first_of(required_modes, supported_modes);
  return (it != required_modes.end()) ? *it : VK_PRESENT_MODE_FIFO_KHR;
}

uint32_t ComputeImageCount(const VkSurfaceCapabilitiesKHR &capabilities) {
  auto LIMIT = std::numeric_limits<uint32_t>::max();
  return std::min(capabilities.minImageCount + 1, capabilities.maxImageCount > 0 ? capabilities.maxImageCount : LIMIT);
}

Swapchain::Swapchain(const Window &window) {
  auto native_window = window.GetNativeWindow();
  VK_CHECK(glfwCreateWindowSurface(Instance(), native_window, nullptr, &surface_));

  CreateSwapchain();
  CreateImageViews();
}

VkExtent2D Swapchain::GetSurfaceExtent() const {
  auto surface_capabilities = GetSurfaceCapabilities();
  return surface_capabilities.currentExtent;
}

VkSurfaceCapabilitiesKHR Swapchain::GetSurfaceCapabilities() const {
  VkSurfaceCapabilitiesKHR surface_capabilities{};
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice(), surface_, &surface_capabilities));
  return surface_capabilities;
}

void Swapchain::CreateSwapchain() {

  auto surface_capabilities = GetSurfaceCapabilities();

  auto image_count = ComputeImageCount(surface_capabilities);

  std::vector<Format> required_formats{Format::B8G8R8A8_SRGB, Format::R8G8B8A8_SRGB};

  auto surface_format = SelectSurfaceFormat(surface_, required_formats);

  surface_format_ = static_cast<Format>(surface_format.format);

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
    swapchain_ci.imageFormat = surface_format.format;
    swapchain_ci.imageColorSpace = surface_format.colorSpace;
    swapchain_ci.minImageCount = ComputeImageCount(surface_capabilities);
    swapchain_ci.presentMode = SelectPresentMode(surface_, std::views::single(VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR));
  }

  VK_CHECK(vkCreateSwapchainKHR(Device(), &swapchain_ci, nullptr, &swapchain_));

  images_.resize(image_count);

  VK_CHECK(vkGetSwapchainImagesKHR(Device(), swapchain_, &image_count, images_.data()));
}

void Swapchain::CreateImageViews() {

  image_views_.resize(images_.size());

  auto range = CreateImageSubresourceRange();

  for (uint32_t i = 0; i < image_views_.size(); i++) {
    Image::CreateImageView(images_[i], image_views_[i], surface_format_, ImageViewType::_2D, range);

    CommandBuffer command_buffer(GraphicsCommandPool());
    command_buffer.Begin();
    command_buffer.ImageMemoryBarrier(images_[i], ImageLayout::UNDEFINED, ImageLayout::PRESENT_SRC_KHR, PipelineStage::TOP_OF_PIPE_BIT,
                                      PipelineStage::COLOR_ATTACHMENT_OUTPUT_BIT, range);
    command_buffer.End();
    command_buffer.Submit();
  }
}

VkResult Swapchain::AcquireNextImage(const VkSemaphore semaphore) {
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

VkResult Swapchain::Present(const VkSemaphore *wait_semaphore) {
  VkPresentInfoKHR present_info{};
  {
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = wait_semaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain_;
    present_info.pImageIndices = &current_image_;
  }
  return vkQueuePresentKHR(GraphicsQueue(), &present_info);
}

} // namespace Innsmouth