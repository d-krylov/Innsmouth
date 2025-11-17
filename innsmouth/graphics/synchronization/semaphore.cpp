#include "semaphore.h"

namespace Innsmouth {

Semaphore::Semaphore() {
  VkSemaphoreCreateInfo semaphore_ci{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  VK_CHECK(vkCreateSemaphore(GraphicsContext::Get()->GetDevice(), &semaphore_ci, nullptr, &semaphore_));
}

Semaphore::~Semaphore() {
  // vkDestroySemaphore(Device(), semaphore_, nullptr);
}

Semaphore::Semaphore(Semaphore &&other) noexcept {
  semaphore_ = std::exchange(other.semaphore_, VK_NULL_HANDLE);
}

Semaphore &Semaphore::operator=(Semaphore &&other) noexcept {
  std::swap(semaphore_, other.semaphore_);
  return *this;
}

} // namespace Innsmouth