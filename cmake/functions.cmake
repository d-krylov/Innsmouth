function(compile_shaders SHADER_FILES SHADER_SPIRV_DIR EXCLUDED_SHADER_EXTENSIONS)
  set(GLSL_FLAGS --target-env vulkan1.3)
  foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_EXTENSION ${SHADER} EXT)
    string(SUBSTRING ${SHADER_EXTENSION} 1 -1 EXT_NO_DOT)
    list(FIND EXCLUDED_SHADER_EXTENSIONS ${SHADER_EXTENSION} EXCLUDED_INDEX)
    if(NOT EXCLUDED_INDEX EQUAL -1)
      continue()
    endif()
    file(RELATIVE_PATH RELATIVE_SHADER_PATH "${PROJECT_SOURCE_DIR}/shaders/" ${SHADER})
    set(SHADER_OUTPUT "${SHADER_SPIRV_DIR}/${RELATIVE_SHADER_PATH}.spv")
    get_filename_component(SHADER_OUTPUT_DIR ${SHADER_OUTPUT} DIRECTORY)
    file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})
    list(APPEND SPIRV_FILES ${SHADER_OUTPUT})
    add_custom_command(
      OUTPUT ${SHADER_OUTPUT}
      COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V -c ${GLSL_FLAGS} ${SHADER} -o ${SHADER_OUTPUT}
      DEPENDS ${SHADER}
      COMMENT "Compiling ${SHADER} to ${SHADER_OUTPUT}"
    )
  endforeach()
  add_custom_target(compile_shaders_target ALL DEPENDS ${SPIRV_FILES})
endfunction()

function(collect_source_recursive DIRECTORY_PATH EXTENSION OUT_VARIABLE)
  if(NOT IS_DIRECTORY "${DIRECTORY_PATH}")
    message(FATAL_ERROR "Directory ${DIRECTORY_PATH} does not exist")
  endif()

  file(GLOB_RECURSE found_files
    "${DIRECTORY_PATH}/*${EXTENSION}"
  )

  set(${OUT_VARIABLE} "${found_files}" PARENT_SCOPE)
endfunction()