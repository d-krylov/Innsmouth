#include "imgui_renderer.h"
#include "innsmouth/graphics/command/command_buffer.h"
#include "innsmouth/graphics/presentation/swapchain.h"
#include "innsmouth/core/include/core.h"
#include "innsmouth/mathematics/include/mathematics_types.h"
#include "imgui.h"

namespace Innsmouth {

ImGuiRenderer::ImGuiRenderer(VkFormat color_format)
  : graphics_pipeline_(GetInnsmouthShadersDirectory() / "gui" / "gui.vert.spv", GetInnsmouthShadersDirectory() / "gui" / "gui.frag.spv",
                       color_format),
    vertex_buffer_(20_MiB, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT), index_buffer_(20_MiB, VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {
  CreateFontsTexture();
}

void ImGuiRenderer::SetBuffers() {
  auto draw_data = ImGui::GetDrawData();
  vertex_buffer_.Map();
  index_buffer_.Map();
  std::size_t vbo_offset = 0, ibo_offset = 0;
  for (const auto &commands : draw_data->CmdLists) {
    std::span<ImDrawVert> vertices(commands->VtxBuffer.Data, commands->VtxBuffer.Size);
    std::span<ImDrawIdx> indices(commands->IdxBuffer.Data, commands->IdxBuffer.Size);
    std::copy(vertices.begin(), vertices.end(), vertex_buffer_.GetMappedData<ImDrawVert>().begin());
    std::copy(indices.begin(), indices.end(), index_buffer_.GetMappedData<ImDrawIdx>().begin());
    vbo_offset += vertices.size();
    ibo_offset += indices.size();
  }
  vertex_buffer_.Unmap();
  index_buffer_.Unmap();
}

void ImGuiRenderer::SetupRenderState(CommandBuffer &command_buffer) {
  auto &io = ImGui::GetIO();
  auto draw_data = ImGui::GetDrawData();

  command_buffer.CommandSetCullMode(VK_CULL_MODE_NONE);
  command_buffer.CommandSetFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE);
  command_buffer.CommandBindGraphicsPipeline(graphics_pipeline_.GetPipeline());
  command_buffer.CommandEnableDepthTest(false);
  command_buffer.CommandEnableDepthWrite(false);

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

  command_buffer.CommandPushConstants(graphics_pipeline_.GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, gui_push_constants_);
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
  font_image_ = std::make_unique<Image>(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM,
                                        ImageInformation(width, height));
  font_image_->SetData<unsigned char>(data);
  io.Fonts->SetTexID((ImTextureID)(intptr_t)font_image_.get());
}

void ImGuiRenderer::Begin(CommandBuffer &command_buffer, const Swapchain &swapchain) {
  ImGui::NewFrame();

  std::array<VkRenderingAttachmentInfo, 1> rendering_ai = {};
  {
    rendering_ai[0].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    rendering_ai[0].imageView = swapchain.GetCurrentImageView();
    rendering_ai[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    rendering_ai[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    rendering_ai[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
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