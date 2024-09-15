set(TEST_NAME test)

add_executable(${TEST_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)

target_link_libraries(${TEST_NAME} PRIVATE Innsmouth)
