# src/CMakeLists.txt
add_executable(csnake main.c)

target_link_libraries(csnake PRIVATE SDL3::SDL3)

set_target_properties(csnake PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
