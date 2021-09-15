cmake_minimum_required(VERSION 3.16)

project(zvfelib VERSION 0.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
add_compile_options(-O0)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
enable_testing()
add_subdirectory(source)
