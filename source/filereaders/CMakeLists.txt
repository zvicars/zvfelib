add_library (FILEREADER STATIC
  timeseries.cpp
  OPFileReader.cpp
  PLUMEDFileReader.cpp
)
target_compile_options(FILEREADER PRIVATE)
target_link_libraries(FILEREADER PRIVATE TOOLS)
target_include_directories(FILEREADER PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory(test)
