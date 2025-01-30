function(compile_shaders SHADER_FILES SHADER_SPIRV_DIR)

  message("${SHADER_SPIRV_DIR}")

  foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)

    set(SHADER_OUTPUT "${SHADER_SPIRV_DIR}/${SHADER_NAME}.spv")

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