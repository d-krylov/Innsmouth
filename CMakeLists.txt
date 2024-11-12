cmake_minimum_required(VERSION 3.27)

project(Innsmouth LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

include(cmake/ClangFormat.cmake)
include(cmake/Libraries.cmake)

FetchContent_MakeAvailable(glm)
FetchContent_MakeAvailable(volk)
FetchContent_MakeAvailable(imgui)
FetchContent_MakeAvailable(vulkan_memory_allocator)
FetchContent_MakeAvailable(spirv_reflect)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

find_package(glfw3 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-volatile")

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

add_subdirectory(innsmouth)

set(SHADER_FILES
  ${PROJECT_SOURCE_DIR}/shaders/sources/mesh/mesh.vert
  ${PROJECT_SOURCE_DIR}/shaders/sources/mesh/mesh.frag
  ${PROJECT_SOURCE_DIR}/shaders/sources/gui/gui.vert
  ${PROJECT_SOURCE_DIR}/shaders/sources/gui/gui.frag
  ${PROJECT_SOURCE_DIR}/shaders/sources/test/test.vert
  ${PROJECT_SOURCE_DIR}/shaders/sources/test/test.frag
  ${PROJECT_SOURCE_DIR}/shaders/sources/raytracing/canvas.vert
  ${PROJECT_SOURCE_DIR}/shaders/sources/raytracing/raytracing.frag
)

set(SHADER_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/shaders/spirv)

foreach(SHADER ${SHADER_FILES})
  get_filename_component(SHADER_NAME ${SHADER} NAME)

  set(SHADER_OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv")

  add_custom_command(
    OUTPUT ${SHADER_OUTPUT}
    COMMAND ${Vulkan_GLSLC_EXECUTABLE} -c --target-env=vulkan1.3 ${SHADER} -o ${SHADER_OUTPUT}
    DEPENDS ${SHADER}
    COMMENT "Compiling ${SHADER} to ${SHADER_OUTPUT}"
  )

  list(APPEND SPIRV_FILES ${SHADER_OUTPUT})

endforeach()

add_custom_target(shaders ALL DEPENDS ${SPIRV_FILES})

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  
  include(cmake/ClangFormat.cmake)

  add_clang_format_target(format "build;external")

  add_subdirectory(examples)

  add_subdirectory(test)

endif()

