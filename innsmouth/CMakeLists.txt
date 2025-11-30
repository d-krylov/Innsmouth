set(INNSMOUTH_SOURCE_DIR ${PROJECT_SOURCE_DIR}/innsmouth)


collect_source_recursive("${INNSMOUTH_SOURCE_DIR}/graphics" ".cpp" INNSMOUTH_GRAPHICS_SOURCES)

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
  ${INNSMOUTH_SOURCE_DIR}/core/image_wrapper.cpp
)

set(INNSMOUTH_SCENE_SOURCES
  ${INNSMOUTH_SOURCE_DIR}/scene/camera.cpp
)

set(INNSMOUTH_SOURCES
  ${INNSMOUTH_GUI_SOURCES}
  ${INNSMOUTH_GRAPHICS_SOURCES}
  ${INNSMOUTH_CORE_SOURCES}
  ${INNSMOUTH_ASSET_SOURCES}
  ${INNSMOUTH_SCENE_SOURCES}
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

target_include_directories(${PROJECT_NAME} 
  PUBLIC ${PROJECT_SOURCE_DIR}
  PUBLIC ${PROJECT_BINARY_DIR}
  PUBLIC ${Vulkan_INCLUDE_DIR}
  PUBLIC ${stb_SOURCE_DIR}
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