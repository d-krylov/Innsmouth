cmake_minimum_required(VERSION 3.27)

project(Innsmouth LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

include(cmake/clang_format.cmake)
include(cmake/libraries.cmake)
include(cmake/functions.cmake)
  
FetchContent_MakeAvailable(
  glm
  volk
  imgui
  vulkan_memory_allocator
  spirv_reflect
  tinyobjloader
  stb
)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

find_package(glfw3 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-volatile")

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

add_subdirectory(innsmouth)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  
  add_clang_format_target(format "build")

  add_subdirectory(examples)

endif()

file(GLOB SHADER_FILES 
  "${PROJECT_SOURCE_DIR}/shaders/sources/gui/*"
  "${PROJECT_SOURCE_DIR}/shaders/sources/tracing/*"
  "${PROJECT_SOURCE_DIR}/shaders/sources/mesh/*"
  ".vert" ".frag"
)

compile_shaders("${SHADER_FILES}" "${PROJECT_SOURCE_DIR}/shaders/spirv")