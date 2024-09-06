cmake_minimum_required(VERSION 3.27)

project(Innsmouth LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

include(cmake/ClangFormat.cmake)
include(cmake/Libraries.cmake)

FetchContent_MakeAvailable(glm)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

find_package(glfw3 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-volatile")

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

add_subdirectory(${PROJECT_SOURCE_DIR}/external/volk)

add_subdirectory(innsmouth)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  
  include(cmake/ClangFormat.cmake)

  add_clang_format_target(format "build;external")

  add_subdirectory(examples)

endif()

