add_library (ANALYSIS STATIC
  Dataset.cpp
  generators/Generator.cpp
  generators/ListGenerator.cpp
  biases/HarmonicBias.cpp
  calculations/Calculation.cpp
  calculations/BWHAM.cpp
  calculations/UBWHAM.cpp
)
target_compile_options(ANALYSIS PRIVATE)
target_link_libraries(ANALYSIS PRIVATE TOOLS FILEREADER)
target_include_directories(ANALYSIS PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
