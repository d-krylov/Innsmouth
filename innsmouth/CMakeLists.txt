set(INNSMOUTH_SOURCE_DIR ${PROJECT_SOURCE_DIR}/innsmouth)

set(IMGUI_SOURCES
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui.cpp
)

set(EXTERNAL_SOURCES
  ${IMGUI_SOURCES}
  ${spirv_reflect_SOURCE_DIR}/spirv_reflect.cpp
  ${PROJECT_SOURCE_DIR}/external/stb/stb.cpp
)

set(INNSMOUTH_CORE_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/core/core_image.cpp
)

set(INNSMOUTH_GRAPHICS_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/graphics/graphics.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/graphics_types.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/graphics_tools.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/pipeline/graphics_pipeline.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/pipeline/compute_pipeline.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/pipeline/shader_module.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/swapchain.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/image/image.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/image/image2D.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/image/image3D.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/buffer/buffer.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/buffer/command_buffer.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/synchronization/semaphore.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/synchronization/fence.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/structure_tools.cpp
)

set(INNSMOUTH_GUI_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/gui/window.cpp
  ${INNSMOUTH_SOURCE_DIR}/gui/window_callback.cpp
  ${INNSMOUTH_SOURCE_DIR}/gui/imgui/imgui_layer.cpp
  ${INNSMOUTH_SOURCE_DIR}/gui/imgui/imgui_renderer.cpp
)

set(INNSMOUTH_SCENE_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/scene/camera.cpp
)

set(INNSMOUTH_MESH_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/mesh/wavefront_loader.cpp
)

set(INNSMOUTH_SOURCES
  ${EXTERNAL_SOURCES}
  ${INNSMOUTH_GUI_SOURCES}
  ${INNSMOUTH_GRAPHICS_SOURCES}
  ${INNSMOUTH_SCENE_SOURCES}
  ${INNSMOUTH_CORE_SOURCES}
  ${INNSMOUTH_MESH_SOURCES}
  ${INNSMOUTH_SOURCE_DIR}/application/application.cpp
)

add_library(${PROJECT_NAME} STATIC
  ${INNSMOUTH_SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${spirv_reflect_SOURCE_DIR}
  ${imgui_SOURCE_DIR}
  ${INNSMOUTH_SOURCE_DIR}
  ${stb_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} 
  glfw
  glm::glm
  volk
  tinyobjloader
)

target_compile_definitions(${PROJECT_NAME} PUBLIC INNSMOUTH_ROOT="${PROJECT_SOURCE_DIR}")