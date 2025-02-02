#ifndef INNSMOUTH_COMMAND_BUFFER_IPP
#define INNSMOUTH_COMMAND_BUFFER_IPP

namespace Innsmouth {

template <typename T>
void CommandBuffer::CommandPushConstants(const GraphicsPipeline &graphics_pipeline, ShaderStage stage, const T &data, uint32_t offset) {
  vkCmdPushConstants(command_buffer_, graphics_pipeline.GetPipelineLayout(), std::to_underlying(stage), offset, sizeof(T), &data);
}

} // namespace Innsmouth

#endif // INNSMOUTH_COMMAND_BUFFER_IPP