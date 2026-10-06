# src/CMakeLists.txt
add_executable(test main.c)

target_link_libraries(test PRIVATE sdl3::SDL3)

set_target_properties(test PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
