#include "innsmouth/common/innsmouth.h"
#include <print>

using namespace Innsmouth;

std::filesystem::path model_path;

bool dirty = false;

class RayTracer : public Innsmouth::Layer {
public:
  void OnImGui() override {
    auto position = camera.GetPosition();
    Vector3f temp_rotation = rotation;
    ImGui::Begin("Settings");
    ImGui::DragFloat3("camera position", glm::value_ptr(position));
    ImGui::DragFloat3("model rotation", glm::value_ptr(temp_rotation));
    ImGui::End();
    camera.SetPosition(position);
    dirty = temp_rotation != rotation;
    rotation = temp_rotation;
  }

  void OnSwapchain() override {
    auto &swapchain = Application::Get()->GetSwapchain();
    auto &[width, height] = swapchain.GetExtent();
    target_image = Image2D(width, height, Format::E_R32G32B32A32_SFLOAT, ImageUsageMaskBits::E_SAMPLED_BIT | ImageUsageMaskBits::E_STORAGE_BIT);
  }

  void OnUpdate(CommandBuffer &command_buffer) override {
    auto &swapchain = Application::Get()->GetSwapchain();
    auto extent = swapchain.GetExtent();

    std::array<BottomLevelAccelerationStructureInstances, 1> bottom_instances;

    for (auto x = 0; x < 10; x++) {
      for (auto z = 0; z < 10; z++) {
        Transform transform;
        transform.SetPosition(Vector3f(30.0f * x, 0.0f, 30.0f * z));
        transform.SetOrientation(rotation);
        transform.SetScale(Vector3f(0.2f));
        bottom_instances[0].instances_.emplace_back(transform.GetModelMatrix());
      }
    }

    bottom_instances[0].acceleration_structure_ = blas.GetAccelerationStructure();

    if (dirty) {
      tlas = AccelerationStructure(bottom_instances);
      dirty = false;
    }

    target_image.SetImageLayout(ImageLayout::E_GENERAL, &command_buffer);

    command_buffer.CommandBindPipeline(ray_tracing_pipeline.GetPipeline(), PipelineBindPoint::E_RAY_TRACING_KHR);
    command_buffer.CommandPushDescriptorSet(ray_tracing_pipeline.GetPipelineLayout(), 0, 0, tlas.GetAccelerationStructure(),
                                            PipelineBindPoint::E_RAY_TRACING_KHR);
    command_buffer.CommandPushDescriptorSet(std::array{target_image.GetDescriptor()}, ray_tracing_pipeline.GetPipelineLayout(), 0, 1,
                                            DescriptorType::E_STORAGE_IMAGE, PipelineBindPoint::E_RAY_TRACING_KHR);

    command_buffer.CommandPushDescriptorSet(ray_tracing_pipeline.GetPipelineLayout(), 0, 2, vertex_buffer.GetHandle(),
                                            PipelineBindPoint::E_RAY_TRACING_KHR);

    command_buffer.CommandPushDescriptorSet(ray_tracing_pipeline.GetPipelineLayout(), 0, 3, index_buffer.GetHandle(),
                                            PipelineBindPoint::E_RAY_TRACING_KHR);

    command_buffer.CommandPushConstants(ray_tracing_pipeline.GetPipelineLayout(), ShaderStageMaskBits::E_RAYGEN_BIT_KHR, camera.GetPosition());

    command_buffer.CommandTraceRay(shader_binding_table.raygen_shader_binding_table_, shader_binding_table.miss_shader_binding_table_,
                                   shader_binding_table.hit_shader_binding_table_, extent.width, extent.height, 1);

    target_image.SetImageLayout(ImageLayout::E_SHADER_READ_ONLY_OPTIMAL, &command_buffer);

    std::array<RenderingAttachmentInfo, 1> rendering_ai = {};

    rendering_ai[0].imageView = swapchain.GetCurrentImageView();
    rendering_ai[0].imageLayout = ImageLayout::E_COLOR_ATTACHMENT_OPTIMAL;
    rendering_ai[0].loadOp = AttachmentLoadOp::E_CLEAR;
    rendering_ai[0].storeOp = AttachmentStoreOp::E_STORE;
    rendering_ai[0].clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};

    command_buffer.CommandBeginRendering(extent, rendering_ai);
    command_buffer.CommandSetViewport(0.0f, extent.height, extent.width, -float(extent.height));
    command_buffer.CommandSetScissor(0, 0, extent.width, extent.height);
    command_buffer.CommandBindPipeline(graphics_pipeline_.GetPipeline(), PipelineBindPoint::E_GRAPHICS);

    command_buffer.CommandPushDescriptorSet(std::array{target_image.GetDescriptor()}, graphics_pipeline_.GetPipelineLayout(), 0, 0,
                                            DescriptorType::E_COMBINED_IMAGE_SAMPLER, PipelineBindPoint::E_GRAPHICS);

    command_buffer.CommandDraw(6);
    command_buffer.CommandEndRendering();
  }

  bool OnResize(WindowResizeEvent &event) {
    return true;
  }

  void OnEvent(Event &event) override {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizeEvent>(BIND_FUNCTION(RayTracer::OnResize));
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

    std::array<BottomLevelAccelerationStructureInstances, 1> bottom_instances;

    rotation = Vector3f(0.0f, PI_ / 2.0f, 0.0f);

    for (auto x = 0; x < 10; x++) {
      for (auto z = 0; z < 10; z++) {
        Transform transform;
        transform.SetPosition(Vector3f(30.0f * x, 0.0f, 30.0f * z));
        transform.SetOrientation(rotation);
        transform.SetScale(Vector3f(0.2f));
        bottom_instances[0].instances_.emplace_back(transform.GetModelMatrix());
      }
    }

    bottom_instances[0].acceleration_structure_ = blas.GetAccelerationStructure();

    tlas = AccelerationStructure(bottom_instances);
  }

  void SetBuffers() {
    model = Model(model_path);

    CommandBuffer command_buffer(GraphicsContext::Get()->GetGraphicsQueueIndex());
    Buffer staging(400_MiB, BufferUsageMaskBits::E_TRANSFER_SRC_BIT, AllocationCreateMaskBits::E_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    BufferUsageMask usage = BufferUsageMaskBits::E_SHADER_DEVICE_ADDRESS_BIT | BufferUsageMaskBits::E_TRANSFER_DST_BIT |
                            BufferUsageMaskBits::E_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;

    vertex_buffer = Buffer(400_MiB, BufferUsageMaskBits::E_STORAGE_BUFFER_BIT | usage, {});
    index_buffer = Buffer(400_MiB, BufferUsageMaskBits::E_STORAGE_BUFFER_BIT | usage, {});

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
  }

  void CreateGraphicsPipeline() {
    auto root = GetInnsmouthShadersDirectory();
    GraphicsPipelineSpecification specification;
    specification.shader_paths_ = {root / "tools" / "square.vert.spv", root / "tools" / "square.frag.spv"};
    graphics_pipeline_ = GraphicsPipeline(specification);
  }

  void OnAttach() override {
    auto root = GetInnsmouthShadersDirectory();
    std::vector<ShaderGroupPaths> shader_groups = {{root / "ray" / "mesh.rgen.spv"},
                                                   {root / "ray" / "mesh.rmiss.spv"},
                                                   {root / "ray" / "mesh.rchit.spv"}};
    ray_tracing_pipeline = RayTracingPipeline(shader_groups, 1);

    shader_binding_table = ShaderBindingTable(ray_tracing_pipeline.GetPipeline(), {1, 1, 1, 0});

    SetBuffers();
    BuildAcceleration();
    CreateGraphicsPipeline();

    auto &swapchain = Application::Get()->GetSwapchain();
    auto &[width, height] = swapchain.GetExtent();

    target_image = Image2D(width, height, Format::E_R32G32B32A32_SFLOAT, ImageUsageMaskBits::E_SAMPLED_BIT | ImageUsageMaskBits::E_STORAGE_BIT);
  }

private:
  Buffer vertex_buffer;
  Buffer index_buffer;
  RayTracingPipeline ray_tracing_pipeline;
  GraphicsPipeline graphics_pipeline_;
  ShaderBindingTable shader_binding_table;
  AccelerationStructure blas;
  AccelerationStructure tlas;
  Image2D target_image;
  Camera camera;
  Vector3f rotation;
  Model model;
};

int main(int argc, char **argv) {

  if (argc == 1) return 0;

  model_path = argv[1];

  Application application;

  RayTracer ray_tracer;

  application.AddLayer(&ray_tracer);

  application.Run();

  return 0;
}