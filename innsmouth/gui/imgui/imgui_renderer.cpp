#include "imgui_renderer.h"
#include "application/include/application.h"
#include "graphics/include/structure_tools.h"
#include "core/include/core_types.h"
#include "mathematics/include/mathematical_types.h"
#include "imgui.h"

namespace Innsmouth {

ImGuiRenderer::ImGuiRenderer(Format color_format)
  : graphics_pipeline_(INNSMOUTH_SHADERS_DIR / "gui" / "gui.vert.spv", INNSMOUTH_SHADERS_DIR / "gui" / "gui.frag.spv", color_format,
                       CreateColorBlendAttachmentState(true)),
    vertex_buffer_(20_MiB, BufferUsage::VERTEX_BUFFER_BIT), index_buffer_(20_MiB, BufferUsage::INDEX_BUFFER_BIT) {
  CreateFontsTexture();
}

void ImGuiRenderer::SetBuffers(ImDrawData *draw_data) {
  vertex_buffer_.Map();
  index_buffer_.Map();
  std::size_t vbo_offset = 0, ibo_offset = 0;
  for (const auto &commands : draw_data->CmdLists) {
    std::span<ImDrawVert> vertices(commands->VtxBuffer.Data, commands->VtxBuffer.Size);
    std::span<ImDrawIdx> indices(commands->IdxBuffer.Data, commands->IdxBuffer.Size);
    vertex_buffer_.Memcpy<ImDrawVert>(vertices, vbo_offset);
    index_buffer_.Memcpy<ImDrawIdx>(indices, ibo_offset);
    vbo_offset += vertices.size();
    ibo_offset += indices.size();
  }
  vertex_buffer_.Unmap();
  index_buffer_.Unmap();
}

void ImGuiRenderer::SetupRenderState(CommandBuffer &command_buffer, ImDrawData *draw_data) {
  auto &io = ImGui::GetIO();

  command_buffer.CommandSetCullMode(CullMode::NONE);
  command_buffer.CommandSetFrontFace(FrontFace::COUNTER_CLOCKWISE);
  command_buffer.CommandBindPipeline(graphics_pipeline_);
  command_buffer.CommandEnableDepthTest(false);
  command_buffer.CommandEnableDepthWrite(false);

  bool is_short_index = sizeof(ImDrawIdx) == 2 ? true : false;

  if (draw_data->TotalVtxCount > 0) {
    command_buffer.CommandBindVertexBuffer(vertex_buffer_);
    command_buffer.CommandBindIndexBuffer(index_buffer_, 0, is_short_index);
  }

  command_buffer.CommandSetViewport(0.0f, 0.0f, io.DisplaySize.x, io.DisplaySize.y);

  gui_push_constants_.scale_x = 2.0f / draw_data->DisplaySize.x;
  gui_push_constants_.scale_y = 2.0f / draw_data->DisplaySize.y;
  gui_push_constants_.translate_x = -1.0f - draw_data->DisplayPos.x * gui_push_constants_.scale_x,
  gui_push_constants_.translate_y = -1.0f - draw_data->DisplayPos.y * gui_push_constants_.scale_y;

  command_buffer.CommandPushConstants(graphics_pipeline_.GetPipelineLayout(), ShaderStage::VERTEX_BIT, gui_push_constants_);
}

void ImGuiRenderer::RenderDrawData(CommandBuffer &command_buffer) {
  auto draw_data = ImGui::GetDrawData();
  auto framebuffer_w = (int32_t)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
  auto framebuffer_h = (int32_t)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);

  if (framebuffer_w <= 0 || framebuffer_h <= 0) {
    return;
  }

  if (draw_data->TotalVtxCount > 0) {
    SetBuffers(draw_data);
  }

  SetupRenderState(command_buffer, draw_data);

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

      Image *image = reinterpret_cast<Image *>(command.TextureId);

      if (image) {
        command_buffer.CommandPushDescriptorSet(graphics_pipeline_.GetPipelineLayout(), 0, 0, *image);
      }

      command_buffer.CommandDrawIndexed(command.ElemCount, 1, command.IdxOffset + global_index_offset,
                                        command.VtxOffset + global_vertex_offset, 0);
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
  font_image_ = std::make_unique<Image2D>(width, height, std::as_bytes(data));
  io.Fonts->SetTexID((ImTextureID)(intptr_t)font_image_.get());
}

void ImGuiRenderer::Begin(CommandBuffer &command_buffer) {
  ImGui::NewFrame();
  auto &swapchain = Application::Get().GetSwapchain();
  std::array attachments{CreateRenderingAttachmentInfo(swapchain.GetCurrentImageView(), LoadOperation::LOAD)};
  command_buffer.CommandBeginRendering(swapchain.GetSurfaceExtent(), attachments);
}

void ImGuiRenderer::End(CommandBuffer &command_buffer) {
  ImGui::Render();
  RenderDrawData(command_buffer);
  command_buffer.CommandEndRendering();
}

} // namespace Innsmouth