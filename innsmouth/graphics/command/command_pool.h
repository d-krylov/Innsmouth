#ifndef INNSMOUTH_COMMAND_POOL_H
#define INNSMOUTH_COMMAND_POOL_H

#include "innsmouth/graphics/graphics_context/graphics_context.h"

namespace Innsmouth {

class CommandPool {
public:
  CommandPool(uint32_t queue_family_index, CommandPoolCreateMask mask);

  ~CommandPool();

  CommandPool(const CommandPool &) = delete;
  CommandPool &operator=(const CommandPool &) = delete;

  CommandPool(CommandPool &&other) noexcept;
  CommandPool &operator=(CommandPool &&other) noexcept;

  VkCommandPool GetHandle() const;

  static VkCommandPool CreateCommandPool(uint32_t family_index, CommandPoolCreateMask mask);

private:
  VkCommandPool command_pool_{VK_NULL_HANDLE};
};

} // namespace Innsmouth

#endif // INNSMOUTH_COMMAND_POOL_H