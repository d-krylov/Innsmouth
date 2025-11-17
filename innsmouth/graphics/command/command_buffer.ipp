#ifndef INNSMOUTH_COMMAND_BUFFER_IPP
#define INNSMOUTH_COMMAND_BUFFER_IPP

namespace Innsmouth {

template <typename T>
void CommandBuffer::CommandPushConstants(const VkPipelineLayout layout, ShaderStageMask stage, const T &data, uint32_t offset) {
  vkCmdPushConstants(command_buffer_, layout, stage.GetValue(), offset, sizeof(T), &data);
}

} // namespace Innsmouth

#endif // INNSMOUTH_COMMAND_BUFFER_IPP