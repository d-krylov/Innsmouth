#include "imgui_renderer.h"
#include "innsmouth/graphics/command/command_buffer.h"
#include "innsmouth/graphics/presentation/swapchain.h"
#include "innsmouth/core/include/core.h"
#include "innsmouth/mathematics/include/mathematics_types.h"
#include "imgui.h"

namespace Innsmouth {

ImGuiRenderer::ImGuiRenderer(Format color_format)
  : vertex_buffer_(20_MiB, BufferUsageMaskBits::E_VERTEX_BUFFER_BIT), index_buffer_(20_MiB, BufferUsageMaskBits::E_INDEX_BUFFER_BIT) {

  auto shader_directory = GetInnsmouthShadersDirectory();
  PipelineSpecification specification;
  specification.color_formats_ = {color_format};
  specification.shader_paths_ = {shader_directory / "gui" / "gui.vert.spv", shader_directory / "gui" / "gui.frag.spv"};
  specification.dynamic_states_.emplace_back(DynamicState::E_CULL_MODE);
  graphics_pipeline_ = GraphicsPipeline(specification);
  CreateFontsTexture();
}

void ImGuiRenderer::SetBuffers() {
  auto draw_data = ImGui::GetDrawData();
  std::size_t vbo_offset = 0, ibo_offset = 0;
  for (const auto &commands : draw_data->CmdLists) {
    std::span<ImDrawVert> vertices(commands->VtxBuffer.Data, commands->VtxBuffer.Size);
    std::span<ImDrawIdx> indices(commands->IdxBuffer.Data, commands->IdxBuffer.Size);
    vertex_buffer_.SetData<ImDrawVert>(vertices, vbo_offset);
    index_buffer_.SetData<ImDrawIdx>(indices, ibo_offset);
    vbo_offset += vertices.size();
    ibo_offset += indices.size();
  }
}

void ImGuiRenderer::SetupRenderState(CommandBuffer &command_buffer) {
  auto &io = ImGui::GetIO();
  auto draw_data = ImGui::GetDrawData();

  command_buffer.CommandBindGraphicsPipeline(graphics_pipeline_.GetPipeline());

  command_buffer.CommandSetCullMode(CullModeMaskBits::E_NONE);

  auto index_type = sizeof(ImDrawIdx) == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;

  if (draw_data->TotalVtxCount > 0) {
    command_buffer.CommandBindVertexBuffer(vertex_buffer_.GetHandle(), 0);
    command_buffer.CommandBindIndexBuffer(index_buffer_.GetHandle(), 0, index_type);
  }

  command_buffer.CommandSetViewport(0.0f, 0.0f, io.DisplaySize.x, io.DisplaySize.y);

  gui_push_constants_.scale_x = 2.0f / draw_data->DisplaySize.x;
  gui_push_constants_.scale_y = 2.0f / draw_data->DisplaySize.y;
  gui_push_constants_.translate_x = -1.0f - draw_data->DisplayPos.x * gui_push_constants_.scale_x,
  gui_push_constants_.translate_y = -1.0f - draw_data->DisplayPos.y * gui_push_constants_.scale_y;

  command_buffer.CommandPushConstants(graphics_pipeline_.GetPipelineLayout(), ShaderStageMaskBits::E_VERTEX_BIT, gui_push_constants_);
}

void ImGuiRenderer::RenderDrawData(CommandBuffer &command_buffer) {
  auto draw_data = ImGui::GetDrawData();
  auto framebuffer_w = static_cast<int32_t>(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
  auto framebuffer_h = static_cast<int32_t>(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);

  if (framebuffer_w <= 0 || framebuffer_h <= 0) {
    return;
  }

  if (draw_data->TotalVtxCount > 0) {
    SetBuffers();
  }

  SetupRenderState(command_buffer);

  auto clip_offset = draw_data->DisplayPos;
  auto clip_scale = draw_data->FramebufferScale;

  int32_t global_vertex_offset = 0;
  int32_t global_index_offset = 0;

  for (const auto commands : draw_data->CmdLists) {
    for (const auto command : commands->CmdBuffer) {

      Vector2f clip_min((command.ClipRect.x - clip_offset.x) * clip_scale.x, (command.ClipRect.y - clip_offset.y) * clip_scale.y);
      Vector2f clip_max((command.ClipRect.z - clip_offset.x) * clip_scale.x, (command.ClipRect.w - clip_offset.y) * clip_scale.y);

      clip_min = glm::max(Vector2f(0.0f), clip_min);
      clip_max = glm::min(clip_max, Vector2f(framebuffer_w, framebuffer_h));

      if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y) {
        continue;
      }

      command_buffer.CommandSetScissor(clip_min.x, clip_min.y, clip_max.x - clip_min.x, clip_max.y - clip_min.y);

      Image *image = reinterpret_cast<Image *>(command.GetTexID());

      if (image != nullptr) {
        command_buffer.CommandPushDescriptorSet(graphics_pipeline_.GetPipelineLayout(), 0, 0, image->GetImageView(), image->GetSampler());
      }

      command_buffer.CommandDrawIndexed(command.ElemCount, 1, command.IdxOffset + global_index_offset, command.VtxOffset + global_vertex_offset,
                                        0);
    }
    global_index_offset += commands->IdxBuffer.Size;
    global_vertex_offset += commands->VtxBuffer.Size;
  }
  command_buffer.CommandSetScissor(0, 0, (uint32_t)framebuffer_w, (uint32_t)framebuffer_h);
}

void ImGuiRenderer::CreateFontsTexture() {
  auto &io = ImGui::GetIO();
  uint8_t *pixels{nullptr};
  int32_t width, height, channels;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &channels);
  auto data = std::span(pixels, width * height * channels);

  ImageSpecification image_specification;
  image_specification.extent_ = Extent3D(width, height, 1);
  image_specification.format_ = Format::E_R8G8B8A8_UNORM;
  image_specification.image_type_ = ImageType::E_2D;
  image_specification.view_type_ = ImageViewType::E_2D;
  image_specification.usage_ = ImageUsageMaskBits::E_SAMPLED_BIT | ImageUsageMaskBits::E_TRANSFER_DST_BIT;
  font_image_ = Image(image_specification, SamplerSpecification());
  font_image_.SetImageData(std::as_bytes(data));
  io.Fonts->SetTexID((ImTextureID)(intptr_t)&font_image_);
}

void ImGuiRenderer::Begin(CommandBuffer &command_buffer, const Swapchain &swapchain) {
  ImGui::NewFrame();

  std::array<RenderingAttachmentInfo, 1> rendering_ai = {};
  {
    rendering_ai[0].imageView = swapchain.GetCurrentImageView();
    rendering_ai[0].imageLayout = ImageLayout::E_COLOR_ATTACHMENT_OPTIMAL;
    rendering_ai[0].loadOp = AttachmentLoadOp::E_LOAD;
    rendering_ai[0].storeOp = AttachmentStoreOp::E_STORE;
    rendering_ai[0].clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};
  }

  command_buffer.CommandBeginRendering(swapchain.GetExtent(), rendering_ai);
}

void ImGuiRenderer::End(CommandBuffer &command_buffer) {
  ImGui::Render();
  RenderDrawData(command_buffer);
  command_buffer.CommandEndRendering();
}

} // namespace Innsmouth