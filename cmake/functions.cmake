function(compile_shaders SHADER_FILES SHADER_SPIRV_DIR)
  set(GLSL_FLAGS --target-env vulkan1.3)
  foreach(SHADER ${SHADER_FILES})
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