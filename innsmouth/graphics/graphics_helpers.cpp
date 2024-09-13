#include "innsmouth/graphics/include/graphics_helpers.h"
#include "innsmouth/graphics/include/graphics.h"
#include <bit>

namespace Innsmouth {

Format FindSupportedFormat(const std::ranges::input_range auto &formats, ImageTiling tiling,
                           FormatFeature features) {
  VkFormatProperties format_properties;
  auto supported = (tiling == ImageTiling::LINEAR) ? &VkFormatProperties::linearTilingFeatures
                                                   : &VkFormatProperties::optimalTilingFeatures;
  auto it = std::ranges::find_if(formats, [&](VkFormat format) {
    vkGetPhysicalDeviceFormatProperties(PhysicalDevice(), VkFormat(format), &format_properties);
    VkFormatFeatureFlags supported_features = format_properties.*supported;
    return (supported_features & VkFormatFeatureFlagBits(features)) == VkFormatFeatureFlagBits(features);
  });

  return (it != formats.end()) ? *it : Format::UNDEFINED;
}

bool GetSupportedFeatures(const VkPhysicalDevice device, const VkPhysicalDeviceFeatures &required_features) {
  VkPhysicalDeviceFeatures supported_features;
  vkGetPhysicalDeviceFeatures(device, &supported_features);
  constexpr auto n = sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32);
  auto L = std::bit_cast<std::array<VkBool32, n>>(supported_features);
  auto R = std::bit_cast<std::array<VkBool32, n>>(required_features);
  return std::equal(R.begin(), R.end(), L.begin(), [](auto r, auto s) { return !r || s; });
}

bool GetSupportedExtensions(const VkPhysicalDevice device, const std::vector<const char *> &required) {
  auto supported = Enumerate<VkExtensionProperties>(vkEnumerateDeviceExtensionProperties, device, nullptr);
  return std::all_of(required.begin(), required.end(), [&supported](const char *r) {
    return std::any_of(supported.begin(), supported.end(),
                       [r](const auto &e) { return std::string_view(e.extensionName) == r; });
  });
}

std::optional<uint32_t> GetQueueFamily(const VkPhysicalDevice device, VkQueueFlags required) {
  auto properties = Enumerate<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, device);
  auto it = std::find_if(properties.begin(), properties.end(), [required](const auto &property) {
    return (property.queueFlags & required) == required;
  });
  return (it != properties.end()) ? std::make_optional(std::distance(properties.begin(), it)) : std::nullopt;
}

} // namespace Innsmouth