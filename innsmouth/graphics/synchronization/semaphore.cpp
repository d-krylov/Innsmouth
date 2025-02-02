#include "semaphore.h"
#include "graphics/include/graphics.h"

namespace Innsmouth {

Semaphore::Semaphore() {
  VkSemaphoreCreateInfo semaphore_ci{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  VK_CHECK(vkCreateSemaphore(Device(), &semaphore_ci, nullptr, &semaphore_));
}

Semaphore::~Semaphore() {
  // vkDestroySemaphore(Device(), semaphore_, nullptr);
}

} // namespace Innsmouth