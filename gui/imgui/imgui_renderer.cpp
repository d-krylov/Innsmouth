#include "gui/imgui/imgui_renderer.h"
#include "core/include/core_types.h"
#include "easyloggingpp/easylogging++.h"
#include "imgui.h"
#include <cstring>

namespace Innsmouth {

std::vector<VkVertexInputBindingDescription> GetBinding() {
  return std::vector<VkVertexInputBindingDescription>{
    {.stride = sizeof(ImDrawVert), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};
}

std::vector<VkVertexInputAttributeDescription> GetAttributes() {
  std::vector<VkVertexInputAttributeDescription> ret(3);
  {
    ret[0].location = 0;
    ret[0].binding = 0;
    ret[0].format = VK_FORMAT_R32G32_SFLOAT;
    ret[0].offset = offsetof(ImDrawVert, pos);
  }
  {
    ret[1].location = 1;
    ret[1].binding = 0;
    ret[1].format = VK_FORMAT_R32G32_SFLOAT;
    ret[1].offset = offsetof(ImDrawVert, uv);
  }
  {
    ret[2].location = 2;
    ret[2].binding = 0;
    ret[2].format = VK_FORMAT_R8G8B8A8_UNORM;
    ret[2].offset = offsetof(ImDrawVert, col);
  }
  return ret;
}

ImGuiRenderer::ImGuiRenderer(const Swapchain &swapchain)
  : graphics_pipeline_({"./gui.vert.spv", "./gui.frag.spv"}, {swapchain.GetSurfaceFormat().format},
                       Depth::NONE, VK_FORMAT_UNDEFINED, GetAttributes(), GetBinding()),
    vertex_buffer_(BufferUsage::VERTEX_BUFFER, 20_MiB),
    index_buffer_(BufferUsage::INDEX_BUFFER, 20_MiB) {
  CreateFontsTexture();
}

ImGuiRenderer::~ImGuiRenderer() {}

struct PushConstants {
  Vector2f scale_;
  Vector2f translate_;
};

void ImGuiRenderer::SetupRenderState(CommandBuffer &command_buffer, ImDrawData *draw_data) {
  auto &io = ImGui::GetIO();

  command_buffer.CommandSetCullMode(false, false);
  command_buffer.CommandSetFrontFace(FrontFace::COUNTER_CLOCKWISE);
  command_buffer.CommandBindPipeline(graphics_pipeline_);

  if (draw_data->TotalVtxCount > 0) {
    command_buffer.CommandBindVertexBuffer(vertex_buffer_);
    command_buffer.CommandBindIndexBuffer(index_buffer_, 0,
                                          sizeof(ImDrawIdx) == 2
                                            ? VkIndexType::VK_INDEX_TYPE_UINT16
                                            : VkIndexType::VK_INDEX_TYPE_UINT32);
  }

  command_buffer.CommandSetViewport(0.0f, 0.0f, io.DisplaySize.x, io.DisplaySize.y);

  PushConstants pc;
  pc.scale_ = Vector2f(2.0f / draw_data->DisplaySize.x, 2.0f / draw_data->DisplaySize.y),
  pc.translate_ = Vector2f(-1.0f - draw_data->DisplayPos.x * pc.scale_.x,
                           -1.0f - draw_data->DisplayPos.y * pc.scale_.y);

  command_buffer.CommandPushConstants(graphics_pipeline_, ShaderStage::VERTEX,
                                      std::as_bytes(ToSpan(pc)));
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

  auto write_descriptor_set =
    font_image_->GetWriteDescriptorSet(0, DescriptorType::COMBINED_IMAGE_SAMPLER);

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

      VkRect2D scissor;
      {
        scissor.offset.x = (int32_t)(clip_min.x);
        scissor.offset.y = (int32_t)(clip_min.y);
        scissor.extent.width = (uint32_t)(clip_max.x - clip_min.x);
        scissor.extent.height = (uint32_t)(clip_max.y - clip_min.y);
      }
      command_buffer.CommandSetScissor(scissor);

      WriteDescriptorSet *descriptor = reinterpret_cast<WriteDescriptorSet *>(command->TextureId);

      if (descriptor) {
        command_buffer.CommandPushDescriptorSet(graphics_pipeline_, 0,
                                                ToSpan(descriptor->write_descriptor_set_));
      }

      command_buffer.CommandDrawIndexed(command->ElemCount, 1,
                                        command->IdxOffset + global_index_offset,
                                        command->VtxOffset + global_vertex_offset, 0);
    }
    global_index_offset += commands->IdxBuffer.Size;
    global_vertex_offset += commands->VtxBuffer.Size;
  }

  VkRect2D scissor = {{0, 0}, {(uint32_t)framebuffer_w, (uint32_t)framebuffer_h}};
  command_buffer.CommandSetScissor(scissor);
}

void ImGuiRenderer::CreateFontsTexture() {
  auto &io = ImGui::GetIO();

  unsigned char *font_data;
  int32_t width, height;
  io.Fonts->GetTexDataAsRGBA32(&font_data, &width, &height);

  auto size = width * height * 4 * sizeof(char);
  std::span<std::byte> data(ToBytePointer(font_data), size);

  font_image_ = std::make_unique<Image2D>(VkExtent2D{uint32_t(width), uint32_t(height)}, data);

  descriptor_ = font_image_->GetWriteDescriptorSet(0);

  io.Fonts->SetTexID((ImTextureID)(intptr_t)&descriptor_);
}

void ImGuiRenderer::Begin() { ImGui::NewFrame(); }

void ImGuiRenderer::End(CommandBuffer &command_buffer) {
  ImGui::Render();
  RenderDrawData(command_buffer, ImGui::GetDrawData());
}

} // namespace Innsmouth