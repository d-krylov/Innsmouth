set(EXECUTABLES
  raymarching
  wavefront_viewer
)

foreach(exe ${EXECUTABLES})
  add_executable(${exe} ${CMAKE_CURRENT_SOURCE_DIR}/${exe}/${exe}.cpp)
  target_link_libraries(${exe} PRIVATE Innsmouth)
endforeach()