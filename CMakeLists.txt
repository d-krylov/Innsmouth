cmake_minimum_required(VERSION 3.27)

project(Innsmouth LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

include(cmake/ClangFormat.cmake)
include(cmake/Libraries.cmake)

add_clang_format_target(format)

FetchContent_MakeAvailable(glm)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

find_package(glfw3  REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-volatile")

set(CMAKE_BUILD_TYPE Debug)

set(IMGUI_SOURCES
  ${PROJECT_SOURCE_DIR}/external/imgui/sources/imgui_draw.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui/sources/imgui_tables.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui/sources/imgui_widgets.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui/sources/imgui.cpp
)

set(SCENE_SOURCES
  ${PROJECT_SOURCE_DIR}/scene/camera.cpp
)

set(GRAPHICS_SOURCES
  ${PROJECT_SOURCE_DIR}/graphics/graphics_pipeline/graphics_pipeline.cpp
  ${PROJECT_SOURCE_DIR}/graphics/graphics_pipeline/shader_module.cpp
  ${PROJECT_SOURCE_DIR}/graphics/buffer.cpp
  ${PROJECT_SOURCE_DIR}/graphics/swapchain.cpp
  ${PROJECT_SOURCE_DIR}/graphics/command_buffer.cpp
  #${PROJECT_SOURCE_DIR}/graphics/descriptor/descriptor_set.cpp
  ${PROJECT_SOURCE_DIR}/graphics/graphics.cpp
  ${PROJECT_SOURCE_DIR}/graphics/graphics_types.cpp
  ${PROJECT_SOURCE_DIR}/graphics/graphics_description.cpp
  ${PROJECT_SOURCE_DIR}/graphics/synchronization.cpp
  ${PROJECT_SOURCE_DIR}/graphics/image/image.cpp
  ${PROJECT_SOURCE_DIR}/graphics/image/image_2d.cpp
)

set(GUI_SOURCES
  ${PROJECT_SOURCE_DIR}/gui/window.cpp
  ${PROJECT_SOURCE_DIR}/gui/window_callback.cpp
  ${PROJECT_SOURCE_DIR}/gui/imgui/imgui_renderer.cpp
  ${PROJECT_SOURCE_DIR}/gui/imgui/imgui_platform.cpp
)

set(MESH_SOURCES
  ${PROJECT_SOURCE_DIR}/mesh/primitives.cpp
)

set(INNSMOUTH_SOURCES
  #${RENDERER_SOURCES}
  ${GRAPHICS_SOURCES}
  ${MESH_SOURCES}
  ${SCENE_SOURCES}
  ${IMGUI_SOURCES}
  ${GUI_SOURCES}
  ${PROJECT_SOURCE_DIR}/core/tools.cpp
  ${PROJECT_SOURCE_DIR}/core/core_image.cpp
  ${PROJECT_SOURCE_DIR}/external/spirv_reflect/spirv_reflect.c
  ${PROJECT_SOURCE_DIR}/external/easyloggingpp/easylogging++.cc
  ${PROJECT_SOURCE_DIR}/innsmouth/application.cpp
)

set(SHADER_FILES
  ${PROJECT_SOURCE_DIR}/shaders/main.vert
  ${PROJECT_SOURCE_DIR}/shaders/main.frag
  #${PROJECT_SOURCE_DIR}/shaders/test.frag
  ${PROJECT_SOURCE_DIR}/shaders/gui.vert
  ${PROJECT_SOURCE_DIR}/shaders/gui.frag
  #${PROJECT_SOURCE_DIR}/shaders/canvas.vert
  #${PROJECT_SOURCE_DIR}/shaders/canvas.frag
)

add_custom_target(shaders COMMAND ${Vulkan_GLSLC_EXECUTABLE} -c --target-env=vulkan1.0 ${SHADER_FILES})

set(INNSMOUTH_INCLUDES
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/external
  ${PROJECT_SOURCE_DIR}/external/imgui/include
)

add_subdirectory(examples)

add_subdirectory(${PROJECT_SOURCE_DIR}/external/volk)

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
  $ENV{VULKAN_SDK}/lib/libVkLayer_utils.a
)