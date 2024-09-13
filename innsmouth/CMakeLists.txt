set(INNSMOUTH_SOURCE_DIR ${PROJECT_SOURCE_DIR}/innsmouth)

set(EXTERNAL_SOURCES
  ${PROJECT_SOURCE_DIR}/external/imgui/sources/imgui_draw.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui/sources/imgui_tables.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui/sources/imgui_widgets.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui/sources/imgui.cpp
  ${PROJECT_SOURCE_DIR}/external/tinyobjloader/tiny_obj_loader.cc
)

set(SCENE_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/scene/camera.cpp
)

set(RENDERER_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/renderer/renderer.cpp
)

set(WIDGET_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/widgets/camera_widget.cpp
  ${INNSMOUTH_SOURCE_DIR}/widgets/transform_widget.cpp
)

set(GRAPHICS_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/graphics/graphics.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/graphics_types.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/graphics_helpers.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/graphics_constants.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/pipeline/graphics_pipeline.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/pipeline/compute_pipeline.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/pipeline/shader_module.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/pipeline/pipeline.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/buffer/buffer.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/swapchain.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/command_buffer.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/graphics_description.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/synchronization.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/image/image.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/image/image_2d.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/image/depth_image.cpp
)

set(GUI_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/gui/key.cpp
  ${INNSMOUTH_SOURCE_DIR}/gui/window.cpp
  ${INNSMOUTH_SOURCE_DIR}/gui/window_callback.cpp
  ${INNSMOUTH_SOURCE_DIR}/gui/imgui/imgui_renderer.cpp
  ${INNSMOUTH_SOURCE_DIR}/gui/imgui/imgui_layer.cpp
)

set(MESH_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/mesh/primitives.cpp
  ${INNSMOUTH_SOURCE_DIR}/mesh/transform.cpp
  ${INNSMOUTH_SOURCE_DIR}/mesh/wavefront_loader.cpp
)

set(INNSMOUTH_SOURCES
  ${GRAPHICS_SOURCES}
  ${MESH_SOURCES}
  ${SCENE_SOURCES}
  ${EXTERNAL_SOURCES}
  ${GUI_SOURCES}
  ${RENDERER_SOURCES}
  ${WIDGET_SOURCES}
  ${INNSMOUTH_SOURCE_DIR}/core/tools.cpp
  ${INNSMOUTH_SOURCE_DIR}/core/core_image.cpp
  ${INNSMOUTH_SOURCE_DIR}/application/application.cpp
  ${PROJECT_SOURCE_DIR}/external/spirv_reflect/spirv_reflect.c
)

set(INNSMOUTH_INCLUDES
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/external
  ${PROJECT_SOURCE_DIR}/external/imgui/include
)

add_library(${PROJECT_NAME} STATIC
  ${INNSMOUTH_SOURCES}
)

add_dependencies(${PROJECT_NAME} shaders)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${INNSMOUTH_INCLUDES}
)

target_link_libraries(${PROJECT_NAME} 
  glfw
  glm::glm
  volk
  $ENV{VULKAN_SDK}/lib/libVulkanLayerSettings.a
)