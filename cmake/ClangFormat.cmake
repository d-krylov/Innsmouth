set(EXCLUDE_FROM_FORMAT
  "build"
  "external"
)

set(CLANG_FORMAT_OPTIONS "-style=${PROJECT_SOURCE_DIR}/.clang-format")

function(add_clang_format_target TARGET_NAME)
  find_program(CLANG_FORMAT_EXECUTABLE clang-format-11)
  if(NOT CLANG_FORMAT_EXECUTABLE)
    message(FATAL_ERROR "clang-format not found")
  endif()

  file(GLOB_RECURSE SOURCES_FOR_FORMAT 
    LIST_DIRECTORIES false 
    RELATIVE "${PROJECT_SOURCE_DIR}"  
    "*.cpp" "*.h" ".ipp"
    )

  foreach(EXCLUDE_DIR ${EXCLUDE_FROM_FORMAT})
    list(FILTER SOURCES_FOR_FORMAT EXCLUDE REGEX "${EXCLUDE_DIR}/")
  endforeach()

  add_custom_target(format
    COMMAND ${CLANG_FORMAT_EXECUTABLE}
    -i
    ${SOURCES_FOR_FORMAT}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )

endfunction()






