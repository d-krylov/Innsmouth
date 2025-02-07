function(compile_shaders SHADER_FILES SHADER_SPIRV_DIR)

  message("${SHADER_SPIRV_DIR}")

  foreach(SHADER ${SHADER_FILES})

    file(RELATIVE_PATH RELATIVE_SHADER_PATH "${PROJECT_SOURCE_DIR}/shaders/sources" ${SHADER})

    set(SHADER_OUTPUT "${SHADER_SPIRV_DIR}/${RELATIVE_SHADER_PATH}.spv")

    get_filename_component(SHADER_OUTPUT_DIR ${SHADER_OUTPUT} DIRECTORY)
    file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

    add_custom_command(
      OUTPUT ${SHADER_OUTPUT}
      COMMAND ${Vulkan_GLSLC_EXECUTABLE} -c --target-env=vulkan1.3 ${SHADER} -o ${SHADER_OUTPUT}
      DEPENDS ${SHADER}
      COMMENT "Compiling ${SHADER} to ${SHADER_OUTPUT}"
    )

    list(APPEND SPIRV_FILES ${SHADER_OUTPUT})

  endforeach()

  add_custom_target(compile_shaders_target ALL DEPENDS ${SPIRV_FILES})

endfunction()