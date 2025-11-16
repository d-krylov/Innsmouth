set(INNSMOUTH_SOURCE_DIR ${PROJECT_SOURCE_DIR}/innsmouth)

set(INNSMOUTH_GRAPHICS_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/graphics/core/graphics_structures.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/core/structure_tools.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/graphics_context/graphics_context.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/graphics_context/graphics_tools.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/graphics_context/graphics_allocator.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/graphics_context/graphics_types.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/presentation/swapchain.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/command/command_buffer.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/image/image.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/buffer/buffer.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/pipeline/graphics_pipeline.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/pipeline/shader_module.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/synchronization/fence.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/synchronization/semaphore.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/descriptors/descriptor_pool.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/descriptors/descriptor_set.cpp
  ${INNSMOUTH_SOURCE_DIR}/graphics/acceleration/acceleration_structure.cpp
)

set(INNSMOUTH_ASSET_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/asset/model.cpp
  ${INNSMOUTH_SOURCE_DIR}/asset/khronos_loader.cpp
)

set(INNSMOUTH_GUI_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/gui/window/key.cpp
  ${INNSMOUTH_SOURCE_DIR}/gui/window/event.cpp
  ${INNSMOUTH_SOURCE_DIR}/gui/window/window.cpp
  ${INNSMOUTH_SOURCE_DIR}/gui/imgui/imgui_layer.cpp
  ${INNSMOUTH_SOURCE_DIR}/gui/imgui/imgui_renderer.cpp
)

set(INNSMOUTH_CORE_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/core/core.cpp
)

set(INNSMOUTH_SOURCES
  ${INNSMOUTH_GUI_SOURCES}
  ${INNSMOUTH_GRAPHICS_SOURCES}
  ${INNSMOUTH_CORE_SOURCES}
  ${INNSMOUTH_ASSET_SOURCES}
  ${INNSMOUTH_SOURCE_DIR}/application/application.cpp
)

# CONFIG

configure_file(
  ${INNSMOUTH_SOURCE_DIR}/common/innsmouth_config.h.in
  ${CMAKE_BINARY_DIR}/config/innsmouth_config.h
)

# IMGUI

set(IMGUI_SOURCES
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC 
  ${imgui_SOURCE_DIR}
)

# SPIRV

add_library(SPIRV-reflect STATIC ${Vulkan_INCLUDE_DIR}/SPIRV-Reflect/spirv_reflect.c)

# Innsmouth

add_library(${PROJECT_NAME} STATIC
  ${INNSMOUTH_SOURCES}
  
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${Vulkan_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} 
  Vulkan::volk
  imgui
  glfw
  fastgltf
  glm::glm
  SPIRV-reflect
)

target_compile_definitions(${PROJECT_NAME} PUBLIC 
  VK_NO_PROTOTYPES
  GLM_ENABLE_EXPERIMENTAL
)