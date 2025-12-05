#include "innsmouth/common/innsmouth.h"
#include <print>

using namespace Innsmouth;

std::filesystem::path model_path;

struct ModelMatrices {
  Matrix4f projection = Matrix4f(1.0f);
  Matrix4f view = Matrix4f(1.0f);
  Matrix4f model = Matrix4f(1.0f);
};

std::vector<DrawIndexedIndirectCommand> GetIndirectCommandsFromMeshes(std::span<const Mesh> meshes) {
  std::vector<DrawIndexedIndirectCommand> commands;
  for (const auto &mesh : meshes) {
    auto &command = commands.emplace_back();
    command.firstIndex = mesh.indices_offset;
    command.indexCount = mesh.indices_size;
    command.vertexOffset = 0;
    command.firstInstance = 0;
    command.instanceCount = 1;
  }
  return commands;
}

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
    depth_image = ImageDepth(w, h);
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

    Transform transform(Vector3f(0.0f), Vector3f(0.1f));

    matrices.projection = camera.GetProjectionMatrix();
    matrices.view = camera.GetViewMatrix();
    matrices.model = transform.GetModelMatrix();

    command_buffer.CommandBeginRendering(swapchain.GetExtent(), rendering_ai, depth_ai);
    command_buffer.CommandBindPipeline(graphics_pipeline.GetPipeline(), PipelineBindPoint::E_GRAPHICS);
    command_buffer.CommandEnableDepthTest(true);
    command_buffer.CommandEnableDepthWrite(true);
    command_buffer.CommandBindIndexBuffer(index_buffer.GetHandle(), 0);
    command_buffer.CommandPushConstants(graphics_pipeline.GetPipelineLayout(), ShaderStageMaskBits::E_VERTEX_BIT, matrices);
    command_buffer.CommandPushDescriptorSet(graphics_pipeline.GetPipelineLayout(), 0, 0, vertex_buffer.GetHandle(),
                                            PipelineBindPoint::E_GRAPHICS);
    command_buffer.CommandPushDescriptorSet(graphics_pipeline.GetPipelineLayout(), 0, 1, tlas.GetAccelerationStructure(),
                                            PipelineBindPoint::E_GRAPHICS);
    command_buffer.CommandPushDescriptorSet(graphics_pipeline.GetPipelineLayout(), 0, 2, mesh_buffer.GetHandle(), PipelineBindPoint::E_GRAPHICS);
    command_buffer.CommandBindDescriptorSet(graphics_pipeline.GetPipelineLayout(), descriptor_set.GetHandle(), 1);
    command_buffer.CommandSetViewport(0.0f, extent.height, extent.width, -float(extent.height));
    command_buffer.CommandSetScissor(0, 0, extent.width, extent.height);
    command_buffer.CommandDrawIndexedIndirect(indirect_buffer.GetHandle(), 0, model.GetMeshes().size());
    command_buffer.CommandEndRendering();
  }

  void BuildAcceleration() {

    BottomLevelGeometry geometry;
    TriangleGeometrySpecification specification;
    specification.vertices_count_ = model.GetVerticesNumber();
    specification.indices_count_ = model.GetIndicesNumber();
    specification.vbo_offset_ = vertex_buffer.GetBufferAddress();
    specification.ibo_offset_ = index_buffer.GetBufferAddress();
    specification.vertex_stride_ = sizeof(Vertex);
    geometry.AddTriangleGeometry(specification);

    blas = AccelerationStructure(geometry);

    Transform transform(Vector3f(0.0f), Vector3f(0.1f));

    std::array<BottomLevelAccelerationStructureInstances, 1> bottom_instances;
    bottom_instances[0].instances_.emplace_back(transform.GetModelMatrix());
    bottom_instances[0].acceleration_structure_ = blas.GetAccelerationStructure();

    tlas = AccelerationStructure(bottom_instances);
  }

  void OnAttach() override {
    auto &swapchain = Application::Get()->GetSwapchain();
    auto extent = swapchain.GetExtent();
    depth_image = ImageDepth(extent.width, extent.height);
    model = Model(model_path);

    CommandBuffer command_buffer(GraphicsContext::Get()->GetGraphicsQueueIndex());
    Buffer staging(400_MiB, BufferUsageMaskBits::E_TRANSFER_SRC_BIT, AllocationCreateMaskBits::E_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    BufferUsageMask usage = BufferUsageMaskBits::E_SHADER_DEVICE_ADDRESS_BIT | BufferUsageMaskBits::E_TRANSFER_DST_BIT |
                            BufferUsageMaskBits::E_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;

    vertex_buffer = Buffer(400_MiB, BufferUsageMaskBits::E_STORAGE_BUFFER_BIT | usage, {});
    index_buffer = Buffer(400_MiB, BufferUsageMaskBits::E_INDEX_BUFFER_BIT | usage, {});
    indirect_buffer = Buffer(40_MiB, BufferUsageMaskBits::E_INDIRECT_BUFFER_BIT, AllocationCreateMaskBits::E_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
    mesh_buffer = Buffer(40_MiB, BufferUsageMaskBits::E_STORAGE_BUFFER_BIT, AllocationCreateMaskBits::E_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    auto commands = GetIndirectCommandsFromMeshes(model.GetMeshes());
    indirect_buffer.SetData<DrawIndexedIndirectCommand>(commands);

    mesh_buffer.SetData<Mesh>(model.GetMeshes());

    auto offset = model.GetVerticesNumber() * sizeof(Vertex);

    staging.SetData(model.GetVertices());
    staging.SetData(model.GetIndices(), offset);

    command_buffer.Begin();
    command_buffer.CommandCopyBuffer(staging.GetHandle(), vertex_buffer.GetHandle(), 0, 0, offset);
    command_buffer.CommandCopyBuffer(staging.GetHandle(), index_buffer.GetHandle(), offset, 0, model.GetIndicesNumber() * sizeof(uint32_t));

    command_buffer.CommandMemoryBarrier(PipelineStageMaskBits2::E_COPY_BIT, AccessMaskBits2::E_TRANSFER_WRITE_BIT,
                                        PipelineStageMaskBits2::E_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, AccessMaskBits2::E_SHADER_READ_BIT);

    command_buffer.End();
    command_buffer.Submit();

    BuildAcceleration();

    DescriptorPoolSize descriptor_pool_size[1];
    descriptor_pool_size[0].type = DescriptorType::E_COMBINED_IMAGE_SAMPLER;
    descriptor_pool_size[0].descriptorCount = model.GetImages().size();

    descriptor_pool = DescriptorPool(descriptor_pool_size, DescriptorPoolCreateMaskBits::E_UPDATE_AFTER_BIND_BIT, 1);

    std::vector<DescriptorImageInfo> image_infos;
    for (const auto &image : model.GetImages()) {
      image_infos.emplace_back(image.GetDescriptor());
    }

    auto shader_directory = GetInnsmouthShadersDirectory();

    GraphicsPipelineSpecification pipeline_specification;
    pipeline_specification.color_formats_ = {swapchain.GetFormat()};
    pipeline_specification.depth_format_ = Format::E_D32_SFLOAT;
    pipeline_specification.dynamic_states_.emplace_back(DynamicState::E_DEPTH_TEST_ENABLE);
    pipeline_specification.dynamic_states_.emplace_back(DynamicState::E_DEPTH_WRITE_ENABLE);
    pipeline_specification.shader_paths_ = {shader_directory / "mesh" / "mesh.vert.spv", shader_directory / "mesh" / "mesh_indirect.frag.spv"};
    graphics_pipeline = GraphicsPipeline(pipeline_specification);

    descriptor_set = DescriptorSet(descriptor_pool.GetHandle(), graphics_pipeline.GetDescriptorSetLayouts()[1], model.GetImages().size());
    descriptor_set.Update(image_infos, 0, DescriptorType::E_COMBINED_IMAGE_SAMPLER, 0);
  }

private:
  ImageDepth depth_image;
  Buffer vertex_buffer;
  Buffer index_buffer;
  Buffer indirect_buffer;
  Buffer mesh_buffer;
  GraphicsPipeline graphics_pipeline;
  Camera camera;
  Model model;
  ModelMatrices matrices;
  DescriptorPool descriptor_pool;
  DescriptorSet descriptor_set;
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