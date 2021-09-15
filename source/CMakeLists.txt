add_subdirectory(tools)
add_subdirectory(filereaders)
add_subdirectory(analysis)

add_executable(zvfelib main.cpp)
target_compile_options(zvfelib PRIVATE -O0)
target_link_libraries(zvfelib PRIVATE ANALYSIS TOOLS FILEREADER)
