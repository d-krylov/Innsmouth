cmake_minimum_required(VERSION 3.27)

project(Innsmouth LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

include(cmake/clang_format.cmake)
include(cmake/libraries.cmake)
include(cmake/functions.cmake)

find_package(Vulkan REQUIRED COMPONENTS volk)

FetchContent_MakeAvailable(
  imgui
  fastgltf
  glm
  stb
)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(innsmouth)

if(${PROJECT_IS_TOP_LEVEL})
  
  add_clang_format_target(format "build")

  add_subdirectory(examples)
  #add_subdirectory(tests)

endif()

file(GLOB SHADER_FILES 
  "${PROJECT_SOURCE_DIR}/shaders/gui/*"
  "${PROJECT_SOURCE_DIR}/shaders/mesh/*"
  "${PROJECT_SOURCE_DIR}/shaders/tests/*"
  ".vert" ".frag"
)

compile_shaders("${SHADER_FILES}" "${PROJECT_BINARY_DIR}/spirv")