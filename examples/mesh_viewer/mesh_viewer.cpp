#include "innsmouth/common/innsmouth.h"
#include <print>

using namespace Innsmouth;

std::filesystem::path model_path;

struct ModelMatrices {
  Matrix4f projection = Matrix4f(1.0f);
  Matrix4f view = Matrix4f(1.0f);
  Matrix4f model = Matrix4f(1.0f);
};

class MeshViewer : public Innsmouth::Layer {
public:
  void OnSwapchain() override {
  }

  void OnImGui() override {
    auto position = camera.GetPosition();
    auto yaw = camera.GetYaw();
    auto pitch = camera.GetPitch();
    ImGui::Begin("Camera");
    ImGui::DragFloat3("position", glm::value_ptr(position));
    ImGui::DragFloat("Yaw", &yaw);
    ImGui::DragFloat("Pitch", &pitch);
    ImGui::End();
    camera.SetPosition(position);
    camera.SetYaw(yaw);
    camera.SetPitch(pitch);
  }

  bool OnResize(WindowResizeEvent &event) {
    auto w = event.GetWidth();
    auto h = event.GetHeight();
    camera.SetAspect(float(w) / float(h));
    depth_image_specification.extent_ = Extent3D(w, h, 1);
    depth_image = std::move(Image(depth_image_specification));
    return true;
  }

  void OnEvent(Event &event) override {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizeEvent>(BIND_FUNCTION(MeshViewer::OnResize));
  }

  void OnUpdate(CommandBuffer &command_buffer) override {
    auto &swapchain = Application::Get()->GetSwapchain();

    std::array<RenderingAttachmentInfo, 1> rendering_ai = {};
    {
      rendering_ai[0].imageView = swapchain.GetCurrentImageView();
      rendering_ai[0].imageLayout = ImageLayout::E_COLOR_ATTACHMENT_OPTIMAL;
      rendering_ai[0].loadOp = AttachmentLoadOp::E_CLEAR;
      rendering_ai[0].storeOp = AttachmentStoreOp::E_STORE;
      rendering_ai[0].clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};
    }

    RenderingAttachmentInfo depth_ai;
    depth_ai.imageView = depth_image.GetImageView();
    depth_ai.imageLayout = ImageLayout::E_DEPTH_ATTACHMENT_OPTIMAL;
    depth_ai.loadOp = AttachmentLoadOp::E_CLEAR;
    depth_ai.storeOp = AttachmentStoreOp::E_STORE;
    depth_ai.clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};
    depth_ai.clearValue.depthStencil = {1.0f, 0};

    auto extent = swapchain.GetExtent();

    matrices.projection = camera.GetProjectionMatrix();
    matrices.view = camera.GetViewMatrix();
    matrices.model = Matrix4f(1.0f); // glm::rotate(Matrix4f(1.0f), PI_ / 2.0f, Vector3f(0.0f, 1.0f, 0.0f));

    command_buffer.CommandBeginRendering(swapchain.GetExtent(), rendering_ai, depth_ai);
    command_buffer.CommandBindGraphicsPipeline(graphics_pipeline.GetPipeline());
    command_buffer.CommandEnableDepthTest(true);
    command_buffer.CommandEnableDepthWrite(true);
    command_buffer.CommandBindIndexBuffer(index_buffer.GetHandle(), 0);
    command_buffer.CommandPushConstants(graphics_pipeline.GetPipelineLayout(), ShaderStageMaskBits::E_VERTEX_BIT, matrices);
    command_buffer.CommandPushDescriptorSet(graphics_pipeline.GetPipelineLayout(), 0, 0, vertex_buffer.GetHandle());
    command_buffer.CommandPushDescriptorSet(graphics_pipeline.GetPipelineLayout(), 0, 1, tlas.GetAccelerationStructures().front());
    command_buffer.CommandSetViewport(0.0f, extent.height, extent.width, -float(extent.height));
    command_buffer.CommandSetScissor(0, 0, extent.width, extent.height);
    command_buffer.CommandDrawIndexed(model.GetIndicesNumber());
    command_buffer.CommandEndRendering();
  }

  void OnAttach() override {
    auto &swapchain = Application::Get()->GetSwapchain();
    auto extent = swapchain.GetExtent();
    depth_image_specification.extent_ = Extent3D(extent.width, extent.height, 1);
    depth_image_specification.format_ = Format::E_D32_SFLOAT;
    depth_image_specification.image_type_ = ImageType::E_2D;
    depth_image_specification.view_type_ = ImageViewType::E_2D;
    depth_image_specification.usage_ = ImageUsageMaskBits::E_DEPTH_STENCIL_ATTACHMENT_BIT;
    depth_image = Image(depth_image_specification);
    model = Model(model_path);

    CommandBuffer command_buffer(GraphicsContext::Get()->GetGraphicsQueueIndex());
    Buffer staging(100_MiB, BufferUsageMaskBits::E_TRANSFER_SRC_BIT, AllocationCreateMaskBits::E_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    BufferUsageMask usage = BufferUsageMaskBits::E_SHADER_DEVICE_ADDRESS_BIT | BufferUsageMaskBits::E_TRANSFER_DST_BIT |
                            BufferUsageMaskBits::E_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;

    vertex_buffer = Buffer(100_MiB, BufferUsageMaskBits::E_STORAGE_BUFFER_BIT | usage, {});
    index_buffer = Buffer(100_MiB, BufferUsageMaskBits::E_INDEX_BUFFER_BIT | usage, {});

    auto offset = model.GetVerticesNumber() * sizeof(Vertex);

    staging.SetData(model.GetVertices());
    staging.SetData(model.GetIndices(), offset);

    command_buffer.Begin();
    command_buffer.CommandCopyBuffer(staging.GetHandle(), vertex_buffer.GetHandle(), 0, 0, offset);
    command_buffer.CommandCopyBuffer(staging.GetHandle(), index_buffer.GetHandle(), offset, 0, model.GetIndicesNumber() * sizeof(uint32_t));
    command_buffer.End();
    command_buffer.Submit();

    auto shader_directory = GetInnsmouthShadersDirectory();

    BLASSpecification specifications[1];
    specifications[0].indices_number_ = model.GetIndicesNumber();
    specifications[0].indices_offset_ = 0;
    specifications[0].vertices_number_ = model.GetVerticesNumber();
    specifications[0].vertices_offset_ = 0;

    blas = AccelerationStructure(vertex_buffer.GetBufferAddress(), index_buffer.GetBufferAddress(), sizeof(Vertex), specifications);

    TLASInstance instance[1];
    tlas = AccelerationStructure(blas.GetAccelerationStructures(), instance);

    PipelineSpecification pipeline_specification;
    pipeline_specification.color_formats_ = {swapchain.GetFormat()};
    pipeline_specification.depth_format_ = Format::E_D32_SFLOAT;
    pipeline_specification.dynamic_states_.emplace_back(DynamicState::E_DEPTH_TEST_ENABLE);
    pipeline_specification.dynamic_states_.emplace_back(DynamicState::E_DEPTH_WRITE_ENABLE);
    pipeline_specification.shader_paths_ = {shader_directory / "mesh" / "mesh.vert.spv", shader_directory / "mesh" / "mesh.frag.spv"};
    graphics_pipeline = GraphicsPipeline(pipeline_specification);
  }

private:
  Image depth_image;
  Buffer vertex_buffer;
  Buffer index_buffer;
  GraphicsPipeline graphics_pipeline;
  Camera camera;
  Model model;
  ModelMatrices matrices;
  ImageSpecification depth_image_specification;
  AccelerationStructure blas;
  AccelerationStructure tlas;
};

int main(int argc, char **argv) {

  if (argc == 1) return 0;

  model_path = argv[1];

  Application application;

  MeshViewer mesh_viewer;

  application.AddLayer(&mesh_viewer);

  application.Run();

  return 0;
}