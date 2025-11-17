#include "sampler.h"

namespace Innsmouth {

Sampler::Sampler() {
}

Sampler::~Sampler() {
}

Sampler::Sampler(Sampler &&other) noexcept {
  sampler_ = std::exchange(other.sampler_, VK_NULL_HANDLE);
}

Sampler &Sampler::operator=(Sampler &&other) noexcept {
  std::swap(sampler_, other.sampler_);
  return *this;
}

VkSampler Sampler::GetHandle() const {
  return sampler_;
}

VkSampler Sampler::CreateVulkanSampler(const SamplerSpecification &specification) {
  VkSampler sampler = VK_NULL_HANDLE;
  SamplerCreateInfo sampler_ci;
  {
    sampler_ci.magFilter = specification.min_filter_;
    sampler_ci.minFilter = specification.mag_filter_;
    sampler_ci.mipmapMode = specification.mipmap_mode_;
    sampler_ci.addressModeU = specification.address_mode_;
    sampler_ci.addressModeV = specification.address_mode_;
    sampler_ci.addressModeW = specification.address_mode_;
    sampler_ci.mipLodBias = 0.0f;
    sampler_ci.anisotropyEnable = false;
    sampler_ci.maxAnisotropy = 1.0f;
    sampler_ci.compareEnable = false;
    sampler_ci.compareOp = CompareOp::E_ALWAYS;
    sampler_ci.minLod = 0.0f;
    sampler_ci.maxLod = 16.0f;
    sampler_ci.borderColor = BorderColor::E_INT_OPAQUE_BLACK;
  }
  VK_CHECK(vkCreateSampler(GraphicsContext::Get()->GetDevice(), sampler_ci, nullptr, &sampler));
  return sampler;
}

} // namespace Innsmouth