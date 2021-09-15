//can take in a vector of generator names and use them to construct a composite generator that combines all terms
#pragma once
#include "Generator.hpp"
class UnionGenerator{
public:
  //this just loads in basic information regarding the generator
  UnionGenerator(const InputPack& input){
    return;
  }
protected:
  std::vector<Generator*> generators_;
  std::vector<std::string> generator_names_;
};