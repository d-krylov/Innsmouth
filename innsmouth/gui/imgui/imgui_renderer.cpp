#include "innsmouth/gui/imgui/imgui_renderer.h"
#include "imgui.h"
#include "innsmouth/application/include/application.h"
#include "innsmouth/core/include/core_types.h"
#include "innsmouth/core/include/tools.h"
#include <cstring>

namespace Innsmouth {

std::vector<VkVertexInputBindingDescription> GetBinding() {
  return std::vector<VkVertexInputBindingDescription>{
    {.stride = sizeof(ImDrawVert), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};
}

std::vector<VkVertexInputAttributeDescription> GetAttributes() {
  return {{0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, pos)},
          {1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, uv)},
          {2, 0, VK_FORMAT_R8G8B8A8_UNORM, offsetof(ImDrawVert, col)}};
}

ImGuiRenderer::ImGuiRenderer(const Swapchain &swapchain)
  : graphics_pipeline_({SHADER_DIR / "gui.vert.spv", SHADER_DIR / "gui.frag.spv"},
                       {swapchain.GetSurfaceFormat()}, Format::UNDEFINED, GetAttributes(), GetBinding()),
    vertex_buffer_(BufferUsage::VERTEX_BUFFER, 20_MiB), index_buffer_(BufferUsage::INDEX_BUFFER, 20_MiB) {
  CreateFontsTexture();
}

void ImGuiRenderer::SetupRenderState(CommandBuffer &command_buffer, ImDrawData *draw_data) {
  auto &io = ImGui::GetIO();

  command_buffer.CommandSetCullMode(CullMode::NONE);
  command_buffer.CommandSetFrontFace(FrontFace::COUNTER_CLOCKWISE);
  command_buffer.CommandBindPipeline(graphics_pipeline_);
  command_buffer.CommandEnableDepthTest(false);
  command_buffer.CommandEnableDepthWrite(false);

  if (draw_data->TotalVtxCount > 0) {
    command_buffer.CommandBindVertexBuffer(vertex_buffer_);
    command_buffer.CommandBindIndexBuffer(index_buffer_, 0,
                                          sizeof(ImDrawIdx) == 2 ? VkIndexType::VK_INDEX_TYPE_UINT16
                                                                 : VkIndexType::VK_INDEX_TYPE_UINT32);
  }

  command_buffer.CommandSetViewport(0.0f, 0.0f, io.DisplaySize.x, io.DisplaySize.y);

  push_constants_.scale_.x = 2.0f / draw_data->DisplaySize.x;
  push_constants_.scale_.y = 2.0f / draw_data->DisplaySize.y;
  push_constants_.translate_.x = -1.0f - draw_data->DisplayPos.x * push_constants_.scale_.x,
  push_constants_.translate_.y = -1.0f - draw_data->DisplayPos.y * push_constants_.scale_.y;

  command_buffer.CommandPushConstants(graphics_pipeline_, ShaderStage::VERTEX, push_constants_);
}

void ImGuiRenderer::RenderDrawData(CommandBuffer &command_buffer, ImDrawData *draw_data) {
  int32_t framebuffer_w = (int32_t)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
  int32_t framebuffer_h = (int32_t)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
  if (framebuffer_w <= 0 || framebuffer_h <= 0) {
    return;
  }

  if (draw_data->TotalVtxCount > 0) {
    auto vbo_base = vertex_buffer_.Map();
    auto ibo_base = index_buffer_.Map();
    std::size_t vbo_offset = 0, ibo_offset = 0;
    for (uint32_t n = 0; n < draw_data->CmdListsCount; n++) {
      const auto *commands = draw_data->CmdLists[n];
      auto vsize = commands->VtxBuffer.Size * sizeof(ImDrawVert);
      auto isize = commands->IdxBuffer.Size * sizeof(ImDrawIdx);
      std::memcpy(vbo_base.subspan(vbo_offset).data(), commands->VtxBuffer.Data, vsize);
      std::memcpy(ibo_base.subspan(ibo_offset).data(), commands->IdxBuffer.Data, isize);
      vbo_offset += vsize;
      ibo_offset += isize;
    }
    vertex_buffer_.Flush();
    index_buffer_.Flush();
    vertex_buffer_.Unmap();
    index_buffer_.Unmap();
  }

  SetupRenderState(command_buffer, draw_data);

  auto clip_offset = draw_data->DisplayPos;
  auto clip_scale = draw_data->FramebufferScale;

  int32_t global_vertex_offset = 0;
  int32_t global_index_offset = 0;

  for (int n = 0; n < draw_data->CmdListsCount; n++) {
    const auto *commands = draw_data->CmdLists[n];
    for (uint32_t i = 0; i < commands->CmdBuffer.Size; i++) {
      const auto *command = &commands->CmdBuffer[i];

      Vector2f clip_min((command->ClipRect.x - clip_offset.x) * clip_scale.x,
                        (command->ClipRect.y - clip_offset.y) * clip_scale.y);
      Vector2f clip_max((command->ClipRect.z - clip_offset.x) * clip_scale.x,
                        (command->ClipRect.w - clip_offset.y) * clip_scale.y);

      clip_min = glm::max(Vector2f(0.0f), clip_min);
      clip_max = glm::min(clip_max, Vector2f(framebuffer_w, framebuffer_h));

      if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y) {
        continue;
      }

      command_buffer.CommandSetScissor(int32_t(clip_min.x), int32_t(clip_min.y),
                                       uint32_t(clip_max.x - clip_min.x), uint32_t(clip_max.y - clip_min.y));

      Image *image = reinterpret_cast<Image *>(command->TextureId);

      if (image) {
        command_buffer.CommandPushDescriptorSet(graphics_pipeline_, 0, 0, *image);
      }

      command_buffer.CommandDrawIndexed(command->ElemCount, 1, command->IdxOffset + global_index_offset,
                                        command->VtxOffset + global_vertex_offset, 0);
    }
    global_index_offset += commands->IdxBuffer.Size;
    global_vertex_offset += commands->VtxBuffer.Size;
  }
  command_buffer.CommandSetScissor(0, 0, (uint32_t)framebuffer_w, (uint32_t)framebuffer_h);
}

void ImGuiRenderer::CreateFontsTexture() {
  auto &io = ImGui::GetIO();

  std::byte *font_data;
  int32_t width, height;
  io.Fonts->GetTexDataAsRGBA32(reinterpret_cast<unsigned char **>(&font_data), &width, &height);

  auto size = width * height * 4;
  std::span<std::byte> data(font_data, size);

  font_image_ = std::make_unique<Image2D>(width, height, data);

  io.Fonts->SetTexID((ImTextureID)(intptr_t)font_image_.get());
}

void ImGuiRenderer::Begin(CommandBuffer &command_buffer) {
  ImGui::NewFrame();

  auto &swapchain = Application::Get().GetSwapchain();

  std::vector<RenderingAttachment> attachments{
    {LoadOperation::LOAD, StoreOperation::STORE, swapchain.GetCurrentImageView()}};

  command_buffer.CommandBeginRendering(swapchain.GetSurfaceExtent(), attachments);
}

void ImGuiRenderer::End(CommandBuffer &command_buffer) {
  ImGui::Render();
  RenderDrawData(command_buffer, ImGui::GetDrawData());
  command_buffer.CommandEndRendering();
}

} // namespace Innsmouth