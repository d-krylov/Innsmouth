#include "graphics/include/swapchain.h"
#include "graphics/include/graphics.h"
#include "graphics/include/graphics_tools.h"
#include <algorithm>
#include <ranges>
#include <limits>

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

void Swapchain::CreateSwapchain() {
  VkSurfaceCapabilitiesKHR surface_capabilities{};

  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice(), surface_, &surface_capabilities));

  std::vector<Format> required_formats{Format::B8G8R8A8_SRGB, Format::R8G8B8A8_SRGB};

  auto surface_format = SelectSurfaceFormat(surface_, required_formats);

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
}

} // namespace Innsmouth