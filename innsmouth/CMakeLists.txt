set(INNSMOUTH_SOURCE_DIR ${PROJECT_SOURCE_DIR}/innsmouth)

set(INNSMOUTH_GRAPHICS_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/graphics/graphics.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/graphics_types.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/graphics_tools.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/pipeline/graphics_pipeline.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/pipeline/compute_pipeline.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/pipeline/shader_module.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/swapchain.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/image/image.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/buffer/buffer.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/buffer/command_buffer.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/synchronization/semaphore.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/synchronization/fence.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/structure_tools.cpp
  ${spirv_reflect_SOURCE_DIR}/spirv_reflect.cpp
)

set(INNSMOUTH_GUI_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/gui/window.cpp
  ${INNSMOUTH_SOURCE_DIR}/gui/imgui/imgui_layer.cpp
  ${INNSMOUTH_SOURCE_DIR}/gui/imgui/imgui_renderer.cpp
)

set(INNSMOUTH_SOURCES
  ${INNSMOUTH_GUI_SOURCES}
  ${INNSMOUTH_GRAPHICS_SOURCES}
  ${INNSMOUTH_SOURCE_DIR}/application/application.cpp
)

add_library(${PROJECT_NAME} STATIC
  ${INNSMOUTH_SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${spirv_reflect_SOURCE_DIR}
  ${imgui_SOURCE_DIR}
  ${INNSMOUTH_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} 
  glfw
  glm::glm
  volk
)