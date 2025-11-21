#include "swapchain.h"
#include "innsmouth/graphics/command/command_buffer.h"
#include "innsmouth/graphics/synchronization/fence.h"
#include "innsmouth/graphics/core/structure_tools.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <limits>
#include <span>

namespace Innsmouth {

VkSurfaceFormatKHR SelectSurfaceFormat(const VkSurfaceKHR surface, std::span<const VkSurfaceFormatKHR> required_formats) {
  auto predicat = [](const VkSurfaceFormatKHR &a, const VkSurfaceFormatKHR &b) { return a.format == b.format && a.colorSpace == b.colorSpace; };
  auto supported_formats = Enumerate(vkGetPhysicalDeviceSurfaceFormatsKHR, GraphicsContext::Get()->GetPhysicalDevice(), surface);
  auto it = std::ranges::find_first_of(supported_formats, required_formats, predicat);
  return (it != supported_formats.end()) ? *it : supported_formats[0];
}

VkPresentModeKHR SelectPresentMode(const VkSurfaceKHR surface, std::span<const VkPresentModeKHR> required_modes) {
  auto supported_modes = Enumerate(vkGetPhysicalDeviceSurfacePresentModesKHR, GraphicsContext::Get()->GetPhysicalDevice(), surface);
  auto it = std::ranges::find_first_of(required_modes, supported_modes);
  return (it != required_modes.end()) ? *it : VK_PRESENT_MODE_FIFO_KHR;
}

uint32_t ComputeImageCount(const VkSurfaceCapabilitiesKHR &capabilities) {
  constexpr auto infinity = std::numeric_limits<uint32_t>::max();
  return std::min(capabilities.minImageCount + 1, capabilities.maxImageCount > 0 ? capabilities.maxImageCount : infinity);
}

VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(const VkSurfaceKHR surface) {
  VkSurfaceCapabilitiesKHR surface_capabilities{};
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GraphicsContext::Get()->GetPhysicalDevice(), surface, &surface_capabilities));
  return surface_capabilities;
}

void Swapchain::CreateSwapchain() {
  auto surface_capabilities = GetSurfaceCapabilities(surface_);
  auto image_count = ComputeImageCount(surface_capabilities);

  surface_extent_.width = surface_capabilities.currentExtent.width;
  surface_extent_.height = surface_capabilities.currentExtent.height;

  std::array required_present_modes{VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR};

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
    swapchain_ci.imageFormat = surface_format_.format;
    swapchain_ci.imageColorSpace = surface_format_.colorSpace;
    swapchain_ci.minImageCount = ComputeImageCount(surface_capabilities);
    swapchain_ci.presentMode = SelectPresentMode(surface_, required_present_modes);
  }

  images_.resize(image_count);

  VK_CHECK(vkCreateSwapchainKHR(GraphicsContext::Get()->GetDevice(), &swapchain_ci, nullptr, &swapchain_current_));
  VK_CHECK(vkGetSwapchainImagesKHR(GraphicsContext::Get()->GetDevice(), swapchain_current_, &image_count, images_.data()));
}

void Swapchain::CreateImageViews() {
  image_views_.resize(images_.size());

  auto subresource_range = GetImageSubresourceRange();

  CommandBuffer command_buffer(GraphicsContext::Get()->GetGraphicsQueueIndex());

  for (auto i = 0; i < images_.size(); i++) {
    VkImageViewCreateInfo image_view_ci{};
    {
      image_view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      image_view_ci.image = images_[i];
      image_view_ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
      image_view_ci.format = surface_format_.format;
      image_view_ci.subresourceRange = subresource_range;
    }

    VK_CHECK(vkCreateImageView(GraphicsContext::Get()->GetDevice(), &image_view_ci, nullptr, &image_views_[i]));

    command_buffer.Begin();
    command_buffer.CommandImageMemoryBarrier(images_[i], ImageLayout::E_UNDEFINED, ImageLayout::E_PRESENT_SRC_KHR,
                                             PipelineStageMaskBits2::E_TOP_OF_PIPE_BIT, PipelineStageMaskBits2::E_COLOR_ATTACHMENT_OUTPUT_BIT,
                                             subresource_range);
    command_buffer.End();

    VkSubmitInfo submit_info{};
    {
      submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submit_info.commandBufferCount = 1;
      submit_info.pCommandBuffers = command_buffer.get();
    }

    Fence fence({});

    VK_CHECK(vkQueueSubmit(GraphicsContext::Get()->GetGeneralQueue(), 1, &submit_info, fence.GetHandle()));

    fence.Wait();
  }
}

void Swapchain::CreateSurface(GLFWwindow *native_window) {
  VK_CHECK(glfwCreateWindowSurface(GraphicsContext::Get()->GetInstance(), native_window, nullptr, &surface_));

  std::vector<VkSurfaceFormatKHR> required_formats{{VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
                                                   {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}};

  surface_format_ = SelectSurfaceFormat(surface_, required_formats);
}

const Extent2D &Swapchain::GetExtent() const {
  return surface_extent_;
}

Format Swapchain::GetFormat() const {
  return Format(surface_format_.format);
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

const VkImageView Swapchain::GetCurrentImageView() const {
  return image_views_[current_image_index_];
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
  VkPresentInfoKHR present_info{};
  {
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = wait_semaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain_current_;
    present_info.pImageIndices = &current_image_index_;
  }
  return vkQueuePresentKHR(GraphicsContext::Get()->GetGeneralQueue(), &present_info);
}

VkResult Swapchain::AcquireNextImage(const VkSemaphore semaphore) {
  auto ret =
    vkAcquireNextImageKHR(GraphicsContext::Get()->GetDevice(), swapchain_current_, UINT64_MAX, semaphore, nullptr, &current_image_index_);
  return ret;
}

} // namespace Innsmouth