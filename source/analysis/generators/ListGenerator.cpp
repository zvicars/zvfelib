#include "ListGenerator.hpp"
ListGenerator::ListGenerator(const InputPack& input):Generator{input}{
  input.params().readVector("filepaths", ParameterPack::KeyType::Required, filepaths_);
  //if the base class was given a list of biases, then the number of specified files should be the same size
  if(isBiased = 1){
    FANCY_ASSERT(filepaths_.size() == biases_.size(), "Size mismatch between the number of simulations and the number of specified biases");
  }
  else{ //else I need to set the list of biases to the appropriate size but otherwise leave it empty
    biases_.resize(filepaths_.size());
  }
  //at this point I should have a list of timeseries paths and timeseries which can then be loaded
  return;
}