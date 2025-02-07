set(EXECUTABLES
  example_1
  example_2
)

foreach(exe ${EXECUTABLES})
  add_executable(${exe} ${CMAKE_CURRENT_SOURCE_DIR}/${exe}/${exe}.cpp)
  target_link_libraries(${exe} PRIVATE Innsmouth)
endforeach()