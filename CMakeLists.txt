project( C-Reflection )
cmake_minimum_required(VERSION 3.16)
# to build shared library on windows
set (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set (BUILD_SHARED_LIBS=TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory(DumpReflect)
add_subdirectory(Reflect)
add_subdirectory(Sizes)

