#include "command_pool.h"

namespace Innsmouth {

CommandPool::CommandPool(uint32_t queue_family_index, CommandPoolCreateMask mask) {
  command_pool_ = CreateCommandPool(queue_family_index, mask);
}

CommandPool::~CommandPool() {
  vkDestroyCommandPool(GraphicsContext::Get()->GetDevice(), command_pool_, nullptr);
}

CommandPool::CommandPool(CommandPool &&other) noexcept {
  command_pool_ = std::exchange(other.command_pool_, VK_NULL_HANDLE);
}

CommandPool &CommandPool::operator=(CommandPool &&other) noexcept {
  std::swap(command_pool_, other.command_pool_);
  return *this;
}

VkCommandPool CommandPool::GetHandle() const {
  return command_pool_;
}

VkCommandPool CommandPool::CreateCommandPool(uint32_t family_index, CommandPoolCreateMask mask) {
  VkCommandPool command_pool = VK_NULL_HANDLE;
  CommandPoolCreateInfo command_pool_ci;
  command_pool_ci.flags = mask;
  command_pool_ci.queueFamilyIndex = family_index;
  VK_CHECK(vkCreateCommandPool(GraphicsContext::Get()->GetDevice(), command_pool_ci, nullptr, &command_pool));
  return command_pool;
}

} // namespace Innsmouth