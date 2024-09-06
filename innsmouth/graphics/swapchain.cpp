#include "innsmouth/graphics/include/swapchain.h"
#include "innsmouth/graphics/image/image.h"
#include "innsmouth/graphics/include/graphics.h"
#include "innsmouth/graphics/include/synchronization.h"
#include "innsmouth/gui/include/window.h"

namespace Innsmouth {

[[nodiscard]] VkPresentModeKHR
SelectPresentMode(const std::vector<VkPresentModeKHR> &supported_modes,
                  const std::vector<VkPresentModeKHR> &requested_modes) {
  auto it = std::find_first_of(requested_modes.begin(), requested_modes.end(),
                               supported_modes.begin(), supported_modes.end());
  return (it != requested_modes.end()) ? *it : VK_PRESENT_MODE_FIFO_KHR;
}

[[nodiscard]] VkSurfaceFormatKHR
SelectSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &supported,
                    const std::vector<VkFormat> &requested, VkColorSpaceKHR space) {
  auto it = std::find_if(supported.begin(), supported.end(), [&](const auto &f) {
    return std::find(requested.begin(), requested.end(), f.format) != requested.end() &&
           f.colorSpace == space;
  });
  return (it != supported.end()) ? *it : supported.front();
}

[[nodiscard]] uint32_t ComputeImageCount(const VkSurfaceCapabilitiesKHR &capabilities) {
  auto ret = capabilities.minImageCount + 1;
  if (capabilities.maxImageCount > 0 && ret > capabilities.maxImageCount) {
    ret = capabilities.maxImageCount;
  }
  return ret;
}

void Swapchain::CreateSurface(Window &window) {
  window.CreateSurface(Instance(), &surface_);
  auto surface_formats =
    Enumerate<VkSurfaceFormatKHR>(vkGetPhysicalDeviceSurfaceFormatsKHR, PhysicalDevice(), surface_);

  std::vector<VkFormat> required_formats{VkFormat::VK_FORMAT_B8G8R8A8_SRGB,
                                         VkFormat::VK_FORMAT_R8G8B8A8_SRGB};

  auto required_colorspace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

  surface_format_ = SelectSurfaceFormat(surface_formats, required_formats, required_colorspace);

  VkBool32 is_present{VK_FALSE};

  vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice(), GraphicsIndex(), surface_, &is_present);

  CORE_VERIFY(is_present == VK_TRUE);
}

Swapchain::Swapchain(Window &window) {
  CreateSurface(window);
  CreateSwapchain();
  CreateImages();
  CreateImageViews();
}

Swapchain::~Swapchain() {
  vkDestroySwapchainKHR(Device(), swapchain_, nullptr);
  for (auto image_view : image_views_) {
    vkDestroyImageView(Device(), image_view, nullptr);
  }
  vkDestroySurfaceKHR(Instance(), surface_, nullptr);
}

void Swapchain::CreateSwapchain() {
  auto present_modes = Enumerate<VkPresentModeKHR>(vkGetPhysicalDeviceSurfacePresentModesKHR,
                                                   PhysicalDevice(), surface_);

  std::vector<VkPresentModeKHR> required_present_modes{
    VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR};

  VK_CHECK(
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice(), surface_, &surface_capabilities_));

  auto image_count = ComputeImageCount(surface_capabilities_);

  VkSwapchainCreateInfoKHR swapchain_ci{};
  {
    swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_ci.surface = surface_;
    swapchain_ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_ci.clipped = true;
    swapchain_ci.preTransform = surface_capabilities_.currentTransform;
    swapchain_ci.imageExtent = surface_capabilities_.currentExtent;
    swapchain_ci.imageArrayLayers = 1;
    swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_ci.oldSwapchain = swapchain_previous_;
    swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_ci.imageFormat = surface_format_.format;
    swapchain_ci.imageColorSpace = surface_format_.colorSpace;
    swapchain_ci.minImageCount = image_count;
    swapchain_ci.presentMode = SelectPresentMode(present_modes, required_present_modes);
  }

  if (surface_capabilities_.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
    swapchain_ci.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }

  if (surface_capabilities_.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
    swapchain_ci.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }

  VK_CHECK(vkCreateSwapchainKHR(Device(), &swapchain_ci, nullptr, &swapchain_));
}

void Swapchain::CreateImages() {
  auto image_count = ComputeImageCount(surface_capabilities_);
  VK_CHECK(vkGetSwapchainImagesKHR(Device(), swapchain_, &image_count, nullptr));
  images_.resize(image_count);
  VK_CHECK(vkGetSwapchainImagesKHR(Device(), swapchain_, &image_count, images_.data()));
}

void Swapchain::CreateImageViews() {
  image_views_.resize(images_.size());

  auto range = CreateImageSubresourceRange();

  for (uint32_t i = 0; i < image_views_.size(); i++) {
    Image::CreateImageView(images_[i], image_views_[i], surface_format_.format,
                           VkImageViewType::VK_IMAGE_VIEW_TYPE_2D, range);

    Image::TransitionImageLayout(images_[i], ImageLayout::UNDEFINED, ImageLayout::PRESENT_SRC_KHR,
                                 PipelineStage::TOP_OF_PIPE, PipelineStage::COLOR_ATTACHMENT_OUTPUT,
                                 range);
  }
}

VkResult Swapchain::AcquireNextImage(const Semaphore &semaphore) {
  auto res = vkAcquireNextImageKHR(Device(), swapchain_, UINT64_MAX, semaphore, VK_NULL_HANDLE,
                                   &current_image_);

  return res;
}

void Swapchain::Cleanup() {
  for (auto &image_view : image_views_) {
    vkDestroyImageView(Device(), image_view, nullptr);
  }
  vkDestroySwapchainKHR(Device(), swapchain_, nullptr);
}

void Swapchain::Recreate() {
  vkDeviceWaitIdle(Device());
  Cleanup();
  CreateSwapchain();
  CreateImages();
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