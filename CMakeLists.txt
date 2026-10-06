# root CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(SDL3-test LANGUAGES C)

# set the output directory for built objects.
# This makes sure tha the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# add all relevant subdirectories
add_subdirectory(external/SDL3 EXCLUDE_FROM_ALL)
add_subdirectory(src)
